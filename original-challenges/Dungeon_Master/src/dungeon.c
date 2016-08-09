/*
 * Copyright (C) Narf Industries <info@narfindustries.com>
 *
 * Permission is hereby granted, cgc_free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
#include "dungeon.h"
#include "malloc.h"
#include "cbstring.h"
#include "cbstdio.h"
#include "comms.h"
#include "memcpy.h"

char* flag_buf=0;

int cgc_moveEnemies(cgc_Dungeon* dungeon, unsigned int moveNum);
int cgc_moveRight(cgc_Dungeon* dungeon, cgc_Object* object);
int cgc_moveLeft(cgc_Dungeon* dungeon, cgc_Object* object);
void cgc_addRoom(cgc_Object* (*room)[ROOM_HEIGHT][ROOM_WIDTH], const char room_string[ROOM_HEIGHT][ROOM_WIDTH], cgc_Coordinate cornerStone, int moveNum);

/**
* Send message about what killed the player
* 
* @param message String containing kill message
*
* @return None
*/
void cgc_sendKillMessage(const char* message) {

	if(cgc_transmit_all(STDOUT, message, cgc_strlen(message)))
		_terminate(TRANSMIT_ERROR);

}

/**
* Create a generic object at coordinates y, x
* 
* @param symbol the symbol displayed for the object type
* @param id the id number for they object type
* @param y the vertical coordinate the object is located on the map
* @param x the horizontal coordinate the object is located on the map
* @param y_dir the horizontal direction the object is moving
* @param x_dir the vertical direction the object is moving
* @param moveNum the number of moves the player has made prior to the object being created
*
* @return a pointer to the object
*/
cgc_Object* cgc_makeObject(char symbol, unsigned int id, unsigned int y, unsigned int x, unsigned int y_dir, unsigned x_dir, int moveNum) {
	cgc_Object* object=NULL;

	if(!(object = cgc_malloc(sizeof(cgc_Object))))
		_terminate(ALLOCATE_ERROR);

	if(!(object->position = cgc_malloc(sizeof(cgc_Coordinate))))
		_terminate(ALLOCATE_ERROR);

	if(!(object->direction = cgc_malloc(sizeof(cgc_Coordinate))))
		_terminate(ALLOCATE_ERROR);

	object->symbol = symbol;
	object->id = id;
	object->position->x = x;
	object->position->y = y;
	object->direction->x = x_dir;
	object->direction->y = y_dir;
	object->moves = moveNum;

	return object;

}

/**
* Destroy an cgc_Object
* 
* @param object The address of the object to cgc_free
*
* @return None
*/
void cgc_destroyObject(cgc_Object* object) {

	if(!(object = cgc_malloc(sizeof(cgc_Object))))
		_terminate(ALLOCATE_ERROR);

	if(!(object->position = cgc_malloc(sizeof(cgc_Coordinate))))
		_terminate(ALLOCATE_ERROR);

	if(!(object->direction = cgc_malloc(sizeof(cgc_Coordinate))))
		_terminate(ALLOCATE_ERROR);

	object->symbol = 0;
	object->id = 0;
	object->position->x = 0;
	object->position->y = 0;
	object->direction->x = 0;
	object->direction->y = 0;
	object->moves = 0;

	cgc_free(object->position);
	object->position = NULL;
	cgc_free(object->direction);
	object->direction = NULL;
	cgc_free(object);

}

/**
* Add the next room to the dungeon
* 
* @param dungeon A pointer to the dungeon and game info
* @param dungeon_idx the location of the room in the dungeon
* @param last_move The last move made by the player
* @param moveNum The number of moves made by the player
*
* @return None
*/
void cgc_extendDungeon(cgc_Dungeon *dungeon, int dungeon_idx, char last_move, int moveNum) {
	int room_idx=2;
	cgc_Coordinate cornerStone;
	cgc_Room* new_room=NULL;
	cgc_Room* prev_room=NULL;

	if(last_move == dungeon->moveTypes.left)
		room_idx = 0;
	else if(last_move == dungeon->moveTypes.right)
		room_idx = 1;
	else if(last_move == dungeon->moveTypes.jump)
		room_idx = 0;
	else if(last_move == dungeon->moveTypes.jumpleft)
		room_idx = 1;
	else if(last_move == dungeon->moveTypes.jumpright)
		room_idx = 0;
	else if(last_move == dungeon->moveTypes.wait) {
		if(moveNum > SECRET_NUM)
			room_idx = 2;
		else
			room_idx = 1;
	}

	cornerStone.y=0;
	cornerStone.x= (dungeon_idx*ROOM_WIDTH);
	if(!(new_room = cgc_malloc(sizeof(cgc_Room))))
		_terminate(ALLOCATE_ERROR);
	cgc_addRoom(&new_room->contents, room_string[room_idx], cornerStone, moveNum);

	for(prev_room=dungeon->start; prev_room->next!=NULL; prev_room=prev_room->next);
	prev_room->next = new_room;

	// Open door
	cgc_Object* door;
	door = prev_room->contents[ROOM_HEIGHT-3][ROOM_WIDTH-1];
	cgc_destroyObject(door);
	prev_room->contents[ROOM_HEIGHT-3][ROOM_WIDTH-1] = cgc_makeObject(EMPTY_SYM, EMPTY_NUM, ROOM_HEIGHT-3, cornerStone.x-1, 0, 0, moveNum);
}

/**
* Add one to the total number of moves made by player and extend
* dungeon if needed.
* 
* @param dungeon A pointer to the dungeon and game info
* @param player  A pointer to the player object
* @param move The last move made by the player
*
* @return None
*/
void cgc_addMove(cgc_Dungeon *dungeon, cgc_Object* player, int move) {
		const char *flag = (const char*) FLAG_PAGE;

		player->moves++;
		if(player->moves % EXTEND_MOVE == 0) {
			int dungeon_idx = 0;
			char last_move = 0;
			int len = 0;
			char* old_list = NULL;

			// Allocate space for next EXTEND_MOVES moves
			dungeon_idx = player->moves / EXTEND_MOVE;
			old_list = dungeon->moveList;
#ifdef PATCHED_1
			if(!(dungeon->moveList = cgc_malloc((dungeon_idx+2)*EXTEND_MOVE*4+1)))
				_terminate(ALLOCATE_ERROR);
			cgc_bzero(dungeon->moveList, (dungeon_idx+2)*EXTEND_MOVE*4+1);
#else
			if(!(dungeon->moveList = cgc_malloc((dungeon_idx+1)*EXTEND_MOVE*4)))
				_terminate(ALLOCATE_ERROR);
			cgc_bzero(dungeon->moveList, (dungeon_idx+1)*EXTEND_MOVE*4);
#endif

			// Copy old list to new space
			len = cgc_strlen(old_list);
			cgc_memcpy(dungeon->moveList, old_list, len);
			cgc_bzero(old_list, len);

			// If there is dungeon left to build
			last_move = move;
			cgc_extendDungeon(dungeon, dungeon_idx, last_move, player->moves);

			if(!flag_buf) {
				if(!(flag_buf = cgc_malloc(512)))
					_terminate(ALLOCATE_ERROR);
				cgc_bzero(flag_buf, 512);

				for (unsigned int i = 0; i < 10; i++) {
					cgc_sprintf(&flag_buf[i*4], "!H", (unsigned char) *flag++);
				}
			}

			cgc_free(old_list);

		}

}

/**
* Set an cgc_Object by cgc_Coordinate
* NOTE: If the Coordinates are invalid, no changes are made.
* 
* @param start A pointer to the first room in the dungeon
* @param location The Coordinates of the object's new location in the dungeon
* @param object A pointer to the object to set
* 
* @return None
*/
void cgc_setObjectByCoord(cgc_Room* start, cgc_Coordinate location, cgc_Object* object) {
	cgc_Room* room;
	int room_idx;
	int i;

	room_idx = location.x / ROOM_WIDTH;
	for(room=start, i=0; room!=NULL && i<room_idx; room=room->next, i++);
	
	if(room) {
		room->contents[location.y][location.x%ROOM_WIDTH] = object;
		object->position->x = location.x;
		object->position->y = location.y;
	}

}

/**
* Get the room at room_idx from the list of rooms
* 
* @param start A pointer to the first room in the list
* @param room_idx The index of the room to return
*
* @return A pointer to the room found at the index, NULL if room does not exist
*/
cgc_Room* cgc_getRoom(cgc_Room* start, int room_idx) {
	cgc_Room* room;

	if(room_idx < 0)
		return NULL;

	if(room_idx == 0)
		return start;

	int i;
	for(i=0, room=start; room!= NULL && i<room_idx; i++, room=room->next);

	return room;

}

/**
* Get an cgc_Object by cgc_Coordinate
* 
* @param start A pointer to the first room in the dungeon
* @param location The Coordinates of the object in the dungeon
*
* @return A pointer to the object
*/
cgc_Object* cgc_getObjectByCoord(cgc_Room* start, cgc_Coordinate location) {
	cgc_Room* room;
	int room_idx;

	room_idx = location.x / ROOM_WIDTH;
	room = cgc_getRoom(start, room_idx);
	
	if(room) 
		return room->contents[location.y%ROOM_HEIGHT][location.x%ROOM_WIDTH];

	return NULL;
}

/**
* Get an cgc_Object by id
* 
* @param start A pointer to the first room in the dungeon
* @param id the identifcation number of the object type
*
* @return A pointer to the object
*/
cgc_Object* cgc_getObjectById(cgc_Room* start, unsigned int id) {

	for(cgc_Room* room=start; room!=NULL; room=room->next) {
		for(int i=0; i<ROOM_HEIGHT; i++) {
			for(int j=0; j<ROOM_WIDTH; j++) {
				if(room->contents[i][j]->id == id)
					return room->contents[i][j];
			}
		}
	}

	return NULL;
}

/**
* Let the player know they have died
* 
* @param dungeon A pointer to the dungeon and game info
*
* @return None
*/
void cgc_playerDied(cgc_Dungeon dungeon) {
	char buffer[1024];
	cgc_Object* player;
	cgc_size_t len;

	cgc_bzero(buffer, 1024);
	if(!(player = cgc_getObjectById(dungeon.start, PLAYER_NUM)))
		_terminate(OBJECT_NOT_FOUND_ERROR);

	cgc_sprintf(buffer, " at position x:!U y:!U after !U moves\n",
		player->position->x, player->position->y, player->moves);

	len = cgc_strlen(buffer);
	if(cgc_transmit_all(STDOUT, buffer, len))
		_terminate(TRANSMIT_ERROR);

}

/**
* Get the name of the player from the user
* 
* @param None
*
* @return A pointer to the player's name
*/
char* cgc_getName() {
	char buffer[MAX_NAME_SIZE+1];
	char *name;
	cgc_size_t len, bytes;

	cgc_bzero(buffer, MAX_NAME_SIZE+1);
	if(cgc_transmit_all(STDOUT, HIGHSCORE_MSG, cgc_strlen(HIGHSCORE_MSG)))
		_terminate(TRANSMIT_ERROR);

	if(cgc_read_until_delim_or_n(STDOUT, buffer, '\n', MAX_NAME_SIZE, &bytes))
		_terminate(READ_ERROR);

	len = cgc_strlen(buffer);
	if(!(name = cgc_malloc(len+1)))
		_terminate(ALLOCATE_ERROR);

	cgc_bzero(name, len+1);
	cgc_memcpy(name, buffer, len);

	return name;

}

/**
* Add a new score to the ordered list of high scores
* 
* @param highScores A pointer to the first cgc_Score object in the list
* @param newScore A pointer to the new cgc_Score to add
*
* @return A pointer to the first cgc_Score object in the list
*/
cgc_Score* cgc_insertNewScore(cgc_Score* highScores, cgc_Score* newScore) {
	cgc_Score* prevScore=highScores;

	if(highScores == NULL || newScore->score < highScores->score) {
		newScore->next = highScores;
		return newScore;
	}

	for(cgc_Score* score=highScores; score!=NULL; score=score->next) {
		if(newScore->score < score->score) {
			prevScore->next = newScore;
			newScore->next = score;
			return highScores;
		}
		prevScore=score;
	}

	prevScore->next=newScore;

	return highScores;
}

/**
* Add a new high score
* 
* @param dungeon A pointer to the dungeon and game info
* @param moves The number of moves made by the winning player
*
* @return None
*/
void cgc_addHighScore(cgc_Dungeon* dungeon, int moves) {
	cgc_Score* newScore;

	if(!(newScore = cgc_malloc(sizeof(cgc_Score)))) 
		_terminate(ALLOCATE_ERROR);
	cgc_bzero((char*)newScore, sizeof(cgc_Score));
	newScore->name = cgc_getName();
	newScore->score = moves;
	newScore->next = NULL;
	dungeon->highScores = cgc_insertNewScore(dungeon->highScores, newScore);

}

/**
* Let the player know they have won and record score
* 
* @param dungeon A pointer to the dungeon and game info
*
* @return None
*/
void cgc_playerWon(cgc_Dungeon* dungeon) {
	char buffer[1024];
	char* ml_buffer;
	cgc_Object* player;
	cgc_size_t len;

	cgc_bzero(buffer, 1024);
	if(!(player = cgc_getObjectById(dungeon->start, PLAYER_NUM)))
		_terminate(OBJECT_NOT_FOUND_ERROR);

	cgc_sprintf(buffer, "You found the treasure at position x:!U y:!U after !U moves\n",
		player->position->x, player->position->y, player->moves);

	len = cgc_strlen(buffer);
	if(cgc_transmit_all(STDOUT, buffer, len))
		_terminate(TRANSMIT_ERROR);

	len = cgc_strlen(MOVELIST_HDR);
	len += cgc_strlen(dungeon->moveList);
	len += cgc_strlen("\n");

	if(!(ml_buffer = cgc_malloc(len+1))) {
		_terminate(ALLOCATE_ERROR);
	}

	cgc_bzero(ml_buffer, len+1);
	cgc_sprintf(ml_buffer, "!X!X\n", MOVELIST_HDR, dungeon->moveList);

	if(cgc_transmit_all(STDOUT, ml_buffer, len))
		_terminate(TRANSMIT_ERROR);

	cgc_bzero(ml_buffer, len+1);
	cgc_free(ml_buffer);

	cgc_addHighScore(dungeon, player->moves);
}

/**
* Attempt to move the player down one space
* 
* @param dungeon A pointer to the dungeon
* @param object A pointer to the object to move
*
* @return RESULT_WIN if the player has won the game
*         RESULT_DIE if the player has died in the game
*         else 0
*/
int cgc_moveDown(cgc_Dungeon* dungeon, cgc_Object* object) {
	cgc_Coordinate next_space_coord;
	cgc_Object *next_space;
	int result=0;

	next_space_coord.x = object->position->x;
	next_space_coord.y = object->position->y+1;
	if(!(next_space = cgc_getObjectByCoord(dungeon->start, next_space_coord)))
		_terminate(OBJECT_NOT_FOUND_ERROR);
	if(next_space->id == FLOOR_NUM)
		return 0;
	else if(next_space->id == WALL_NUM)
		return 0;
	else if (next_space->id == TREASURE_NUM && object->id == PLAYER_NUM)
		return RESULT_WIN;
	else if (next_space->id == BAT_NUM && object->id == PLAYER_NUM) {
		cgc_sendKillMessage(BAT_KILL_MSG);
		return RESULT_DIE;
	}
	else if (next_space->id == ZOMBIE_NUM && object->id == PLAYER_NUM) {
		cgc_sendKillMessage(ZOMBIE_KILL_MSG);
		return RESULT_DIE;	
	}
	else if (next_space->id == PLAYER_NUM)
		return RESULT_DIE;
	else if (next_space->id == EMPTY_NUM) {
		cgc_Coordinate tmp;
		tmp.x = object->position->x;
		tmp.y = object->position->y;
		cgc_setObjectByCoord(dungeon->start, next_space_coord, object);
		cgc_setObjectByCoord(dungeon->start, tmp, next_space);
	}

	return 0;
}

/**
* Attempt to move the player up one space
* 
* @param dungeon A pointer to the dungeon
* @param object A pointer to the object to move
*
* @return RESULT_WIN if the player has won the game
*         RESULT_DIE if the player has died in the game
*         else 0
*/
int cgc_moveUp(cgc_Dungeon* dungeon, cgc_Object* object) {
	cgc_Coordinate next_space_coord;
	cgc_Object *next_space;
	int result=0;

	next_space_coord.x = object->position->x;
	next_space_coord.y = object->position->y-1;
	if(!(next_space = cgc_getObjectByCoord(dungeon->start, next_space_coord)))
		_terminate(OBJECT_NOT_FOUND_ERROR);

	if(next_space->id == FLOOR_NUM)
		return 0;
	else if(next_space->id == WALL_NUM)
		return 0;
	else if (next_space->id == TREASURE_NUM && object->id == PLAYER_NUM)
		return RESULT_WIN;
	else if (next_space->id == BAT_NUM && object->id == PLAYER_NUM) {
		cgc_sendKillMessage(BAT_KILL_MSG);
		return RESULT_DIE;
	}
	else if (next_space->id == ZOMBIE_NUM && object->id == PLAYER_NUM) {
		cgc_sendKillMessage(ZOMBIE_KILL_MSG);
		return RESULT_DIE;	
	}
	else if (next_space->id == PLAYER_NUM)
		return RESULT_DIE;
	else if (next_space->id == EMPTY_NUM) {
		cgc_Coordinate tmp;
		tmp.x = object->position->x;
		tmp.y = object->position->y;
		cgc_setObjectByCoord(dungeon->start, next_space_coord, object);
		cgc_setObjectByCoord(dungeon->start, tmp, next_space);

		return 0;
	}

	return 0;
}

/**
* Attempt to move the player left one space
* 
* @param dungeon A pointer to the dungeon
* @param object A pointer to the object to move
*
* @return RESULT_WIN if the player has won the game
*         RESULT_DIE if the player has died in the game
*         else 0
*/
int cgc_moveLeft(cgc_Dungeon* dungeon, cgc_Object* object) {
	cgc_Coordinate next_space_coord;
	cgc_Object *next_space;
	int result=0;

	next_space_coord.x = object->position->x-1;
	next_space_coord.y = object->position->y;
	if(!(next_space = cgc_getObjectByCoord(dungeon->start, next_space_coord)))
		_terminate(OBJECT_NOT_FOUND_ERROR);

	if(object->id == PLAYER_NUM)
		cgc_addMove(dungeon, object, dungeon->moveTypes.left);

	if(next_space->id == WALL_NUM) {
		if(object->id == ZOMBIE_NUM || object->id == BAT_NUM)
			object->direction->x = DIR_RIGHT;

		return result;
	} else if(next_space->id == FLOOR_NUM) {
		if(object->id == ZOMBIE_NUM || object->id == BAT_NUM)
			object->direction->x = DIR_RIGHT;

		return result;
	} else if (next_space->id == TREASURE_NUM) {
		if(object->id == ZOMBIE_NUM || object->id == BAT_NUM)
			object->direction->x = DIR_RIGHT;
		if(object->id == PLAYER_NUM)
			return RESULT_WIN;
		return result;		
	} else if (next_space->id == BAT_NUM) {
		if(object->id == PLAYER_NUM) {
	    	cgc_sendKillMessage(BAT_KILL_MSG);
			return RESULT_DIE;
		}	    	
		if(object->id == ZOMBIE_NUM || object->id == BAT_NUM)
			object->direction->x = DIR_RIGHT;	    
		return result;
	}
	else if (next_space->id == ZOMBIE_NUM) {
		if(object->id == PLAYER_NUM) {
	    	cgc_sendKillMessage(ZOMBIE_KILL_MSG);
			return RESULT_DIE;
		}	    	
		if(object->id == ZOMBIE_NUM || object->id == BAT_NUM)
			object->direction->x = DIR_RIGHT;	    
		return result;
	}
	else if (next_space->id == PLAYER_NUM)
		return RESULT_DIE;
	else if (next_space->id == EMPTY_NUM) {
		cgc_Coordinate tmp;
		tmp.x = object->position->x;
		tmp.y = object->position->y;
		cgc_setObjectByCoord(dungeon->start, next_space_coord, object);
		cgc_setObjectByCoord(dungeon->start, tmp, next_space);

		return 0;
	}

	return 0;
}

/**
* Attempt to move the player right one space
* 
* @param dungeon A pointer to the dungeon
* @param object A pointer to the object to move
*
* @return RESULT_WIN if the player has won the game
*         RESULT_DIE if the player has died in the game
*         else 0
*/
int cgc_moveRight(cgc_Dungeon* dungeon, cgc_Object* object) {
	cgc_Coordinate next_space_coord;
	cgc_Object *next_space;
	int result=0;

	next_space_coord.x = object->position->x+1;
	next_space_coord.y = object->position->y;
	if(!(next_space = cgc_getObjectByCoord(dungeon->start, next_space_coord)))
		_terminate(OBJECT_NOT_FOUND_ERROR);

	if(object->id == PLAYER_NUM)
		cgc_addMove(dungeon, object, dungeon->moveTypes.right);

	if(next_space->id == FLOOR_NUM) {
		if(object->id == ZOMBIE_NUM || object->id == BAT_NUM)
			object->direction->x = DIR_LEFT;
		return result;
	} else if(next_space->id == WALL_NUM) {
		if(object->id == ZOMBIE_NUM || object->id == BAT_NUM)
			object->direction->x = DIR_LEFT;
		return result;
	} else if (next_space->id == TREASURE_NUM) {
		if(object->id == ZOMBIE_NUM || object->id == BAT_NUM)
			object->direction->x = DIR_LEFT;
		if(object->id == PLAYER_NUM)
			return RESULT_WIN;
		return result;		
	} else if (next_space->id == BAT_NUM) {
		if(object->id == PLAYER_NUM) {
	    	cgc_sendKillMessage(BAT_KILL_MSG);
			return RESULT_DIE;
		}	    	
		if(object->id == ZOMBIE_NUM || object->id == BAT_NUM)
			object->direction->x = DIR_LEFT;	    
		return result;
	}
	else if (next_space->id == ZOMBIE_NUM) {
		if(object->id == PLAYER_NUM) {
	    	cgc_sendKillMessage(ZOMBIE_KILL_MSG);
			return RESULT_DIE;
		}	    	
		if(object->id == ZOMBIE_NUM || object->id == BAT_NUM)
			object->direction->x = DIR_LEFT;	    
		return result;
	}
	else if (next_space->id == PLAYER_NUM)
		return RESULT_DIE;
	else if (next_space->id == EMPTY_NUM) {
		cgc_Coordinate tmp;
		tmp.x = object->position->x;
		tmp.y = object->position->y;
		cgc_setObjectByCoord(dungeon->start, next_space_coord, object);
		cgc_setObjectByCoord(dungeon->start, tmp, next_space);

		return 0;
	}

	return 0;
}

/**
* Check if the object is standing on a floor object, if not move player
* 
* @param dungeon A pointer to the dungeon and game info
* @param object The object to check
*
* @return RESULT_WIN if the player has won the game
*         RESULT_DIE if the player has died in the game
*         else 0
*/
int cgc_checkFloor(cgc_Dungeon *dungeon, cgc_Object* object) {
	cgc_Object* floor_obj;
	cgc_Coordinate floor_coord;
	int result=0;

	floor_coord.x = object->position->x;
	floor_coord.y = object->position->y+1;
	if(!(floor_obj = cgc_getObjectByCoord(dungeon->start, floor_coord)))
		_terminate(OBJECT_NOT_FOUND_ERROR);

	if(object->direction->x == 0) {
		while(floor_obj->id > WALL_NUM) {
			if((result = cgc_moveDown(dungeon, object)))
				return result;
			if(object->id == PLAYER_NUM) {
				cgc_addMove(dungeon, object, dungeon->moveTypes.left);
				if((result = cgc_moveEnemies(dungeon, object->moves)))
					return result;
				cgc_sendCurrentDungeonView(dungeon->start);
			}
			floor_coord.x = object->position->x;
			floor_coord.y = object->position->y+1;
			if(!(floor_obj = cgc_getObjectByCoord(dungeon->start, floor_coord)))
				_terminate(OBJECT_NOT_FOUND_ERROR);
		}
	} else if(object->direction->x == DIR_LEFT) {
		while(floor_obj->id > WALL_NUM) {
			if((result = cgc_moveDown(dungeon, object)))
				return result;
			if((result = cgc_moveLeft(dungeon, object)))
				return result;
			if(object->id == PLAYER_NUM) {
				if((result = cgc_moveEnemies(dungeon, object->moves)))
					return result;
				cgc_sendCurrentDungeonView(dungeon->start);
			}
			floor_coord.x = object->position->x;
			floor_coord.y = object->position->y+1;
			if(!(floor_obj = cgc_getObjectByCoord(dungeon->start, floor_coord)))
				_terminate(OBJECT_NOT_FOUND_ERROR);
		}
	} else if(object->direction->x == DIR_RIGHT) {
		while(floor_obj->id > WALL_NUM) {
			if((result = cgc_moveDown(dungeon, object)))
				return result;
			if((result = cgc_moveRight(dungeon, object)))
				return result;
			if(object->id == PLAYER_NUM) {
				if((result = cgc_moveEnemies(dungeon, object->moves)))
					return result;
				cgc_sendCurrentDungeonView(dungeon->start);
			}
			floor_coord.x = object->position->x;
			floor_coord.y = object->position->y+1;
			if(!(floor_obj = cgc_getObjectByCoord(dungeon->start, floor_coord)))
				_terminate(OBJECT_NOT_FOUND_ERROR);
		}
	}

	return 0;

}

/**
* Move zombie one space
* 
* @param dungeon A pointer to the dungeon
* @param zombie A pointer to the zombie
* @param moveNum The number of moves the player has made
*
* @return RESULT_DIE if the player has died in the game
*         else 0
*/
int cgc_moveZombie(cgc_Dungeon* dungeon, cgc_Object* zombie, unsigned int moveNum) {
	int result=0;

	if(zombie->moves == moveNum)
		return 0;

	if(zombie->direction->x == DIR_LEFT) {
		if((result = cgc_moveLeft(dungeon, zombie)))
			return result;
		if((result = cgc_checkFloor(dungeon, zombie)))
			return result;
	} else if(zombie->direction->x == DIR_RIGHT) {
		if((result = cgc_moveRight(dungeon, zombie)))
			return result;
		if((result = cgc_checkFloor(dungeon, zombie)))
			return result;
	}

	zombie->moves++;

	return 0;
}

/**
* Move bat one space
* 
* @param dungeon A pointer to the dungeon
* @param bat A pointer to the bat
* @param moveNum The number of moves the player has made
*
* @return RESULT_DIE if the player has died in the game
*         else 0
*/
int cgc_moveBat(cgc_Dungeon* dungeon, cgc_Object* bat, unsigned int moveNum) {
	int result=0;

	if(bat->moves == moveNum)
		return 0;

	if(bat->direction->x == DIR_LEFT) {
		if((result = cgc_moveLeft(dungeon, bat)))
			return result;
	} else if(bat->direction->x == DIR_RIGHT) {
		if((result = cgc_moveRight(dungeon, bat)))
			return result;
	} 

	if(bat->direction->y == DIR_UP) {
		if((result = cgc_moveDown(dungeon, bat)))
			return result;
		bat->direction->y = DIR_DOWN;
	} else {
		if((result = cgc_moveUp(dungeon, bat)))
			return result;
		bat->direction->y = DIR_UP;
	}

	bat->moves++;

	return 0;
}

/**
* Move all enemies in the dungeon one space
* 
* @param dungeon A pointer to the dungeon
* @param moveNum The number of moves the player has made
*
* @return RESULT_DIE if the player has died in the game
*         else 0
*/
int cgc_moveEnemies(cgc_Dungeon* dungeon, unsigned int moveNum) {
	int result = 0;

	for(cgc_Room* room=dungeon->start; room!=NULL; room=room->next) {
		for(int i=0; i<ROOM_HEIGHT; i++) {
			for(int j=0; j<ROOM_WIDTH; j++) {
				
				switch(room->contents[i][j]->id) {
					case BAT_NUM:
						if((result = cgc_moveBat(dungeon, room->contents[i][j], moveNum))) {
							cgc_sendKillMessage(BAT_KILL_MSG);
							return result;
						}
						break;
					case ZOMBIE_NUM:
						if((result = cgc_moveZombie(dungeon, room->contents[i][j], moveNum))) {
							cgc_sendKillMessage(ZOMBIE_KILL_MSG);
							return result;
						}
						break;
					default:
						continue;
				}

			}
		}
	}

	return 0;
}

/**
* Attempt to jump JUMP_HEIGHT spaces up and return to starting position
* NOTE: If there are not JUMP_HEIGHT spaces, player will stay at the highest
*       point until JUMP_HEIGHT moves upward have been made. The player will
*       then move down until the floor is reached.
* 
* @param dungeon A pointer to the dungeon
* @param dungeon A pointer to the object to move
*
* @return RESULT_WIN if the player has won the game
*         RESULT_DIE if the player has died in the game
*         else 0
*/
int jump(cgc_Dungeon* dungeon, cgc_Object* object) {
	int result=0;
	object->direction->x = 0;
	object->direction->y = DIR_UP;
	for(int up=1; up<=JUMP_HEIGHT; up++) {
		if(object->id == PLAYER_NUM)
			cgc_addMove(dungeon, object, dungeon->moveTypes.jump);
		if((result = cgc_moveUp(dungeon, object)))
			return result;
		if((result = cgc_moveEnemies(dungeon, object->moves)))
			return result;
		cgc_sendCurrentDungeonView(dungeon->start);
	}

	object->direction->y = DIR_DOWN;
	if((result = cgc_checkFloor(dungeon, object)))
		return result;

	return 0;
}

/**
* Attempt to jump JUMP_HEIGHT spaces up and to the left
* NOTE: If there are not JUMP_HEIGHT spaces, player will stay at the highest
*       point until JUMP_HEIGHT moves upward have been made. The player will
*       then move down until the floor is reached. Player will move left a 
*       space for each move up and down unless a wall or floor is hit.
* 
* @param dungeon A pointer to the dungeon
* @param dungeon A pointer to the object to move
*
* @return RESULT_WIN if the player has won the game
*         RESULT_DIE if the player has died in the game
*         else 0
*/
int jumpLeft(cgc_Dungeon* dungeon, cgc_Object* object) {
	int result=0;

	object->direction->y = DIR_UP;
	object->direction->x = DIR_LEFT;
	for(int up=1; up<=JUMP_HEIGHT; up++) {
		if((result = cgc_moveUp(dungeon, object)))
			return result;
		if((result = cgc_moveLeft(dungeon, object)))
			return result;
		if((result = cgc_moveEnemies(dungeon, object->moves)))
			return result;
		cgc_sendCurrentDungeonView(dungeon->start);
	}

	object->direction->y = DIR_DOWN;
	if((result = cgc_checkFloor(dungeon, object)))
		return result;

	object->direction->x = 0;

	return 0;
}


/**
* Attempt to jump JUMP_HEIGHT spaces up and to the left
* NOTE: If there are not JUMP_HEIGHT spaces, player will stay at the highest
*       point until JUMP_HEIGHT moves upward have been made. The player will
*       then move down until the floor is reached. Player will move left a 
*       space for each move up and down unless a wall or floor is hit.
* 
* @param dungeon A pointer to the dungeon
* @param dungeon A pointer to the object to move
*
* @return RESULT_WIN if the player has won the game
*         RESULT_DIE if the player has died in the game
*         else 0
*/
int jumpRight(cgc_Dungeon* dungeon, cgc_Object* object) {
	int result=0;

	object->direction->y = DIR_UP;
	object->direction->x = DIR_RIGHT;
	for(int up=1; up<=JUMP_HEIGHT; up++) {
		if((result = cgc_moveUp(dungeon, object)))
			return result;
		if((result = cgc_moveRight(dungeon, object)))
			return result;
		if((result = cgc_moveEnemies(dungeon, object->moves)))
			return result;
		cgc_sendCurrentDungeonView(dungeon->start);
	}

	object->direction->y = DIR_DOWN;
	if((result = cgc_checkFloor(dungeon, object)))
		return result;

	object->direction->x = 0;

	return 0;
}

/**
* Attempt to move the player as instructed
* 
* @param dungeon A pointer to the dungeon
* @param move The move to make
*
* @return RESULT_WIN if the player has won the game
*         RESULT_DIE if the player has died in the game
*         INVALID_MOVE if the move is invalid
*         else 0
*/
int cgc_makeMove(cgc_Dungeon* dungeon, char move) {
	cgc_Object *player;
	int result=0;
	int len;

	if(!dungeon->moveList) {
		dungeon->moveList = cgc_malloc(EXTEND_MOVE*4+1);
		cgc_bzero(dungeon->moveList, EXTEND_MOVE*4+1);
	}

	len = cgc_strlen(dungeon->moveList);
	cgc_sprintf(&dungeon->moveList[len], "!H", (unsigned char) move);

	if(!(player = cgc_getObjectById(dungeon->start, PLAYER_NUM)))
		_terminate(OBJECT_NOT_FOUND_ERROR);

	if(move == dungeon->moveTypes.left) {
		player->direction->x = 0;
		if((result = cgc_moveLeft(dungeon, player)))
			return result;
		if((result = cgc_moveEnemies(dungeon, player->moves)))
			return result;
		cgc_sendCurrentDungeonView(dungeon->start);
		if((result = cgc_checkFloor(dungeon, player)))
			return result;
	} else if(move == dungeon->moveTypes.right) {
		player->direction->x = 0;
		if((result = cgc_moveRight(dungeon, player)))
			return result;
		if((result = cgc_moveEnemies(dungeon, player->moves)))
			return result;
		cgc_sendCurrentDungeonView(dungeon->start);
		if((result = cgc_checkFloor(dungeon, player)))
			return result;
	}
	else if(move == dungeon->moveTypes.jump)
		return jump(dungeon, player);
	else if (move == dungeon->moveTypes.jumpleft)
		return jumpLeft(dungeon, player);
	else if (move == dungeon->moveTypes.jumpright)
		return jumpRight(dungeon, player);
	else if (move == dungeon->moveTypes.wait) {
		cgc_addMove(dungeon, player, dungeon->moveTypes.wait);
		if((result = cgc_moveEnemies(dungeon, player->moves)))
			return result;
		cgc_sendCurrentDungeonView(dungeon->start);
		return 0;
	} else
		return INVALID_MOVE;
	
	return 0;

}

/**
* Send the current cgc_Dungeon view
* NOTE: The current view will only show a partial view of the dungeon
*       based on the players current position.
* 
* @param start A pointer to the first room in the dungeon
*
* @return None
*/
void cgc_sendCurrentDungeonView(cgc_Room* start) {
	cgc_Object* player;
	char view[VIEW_WIDTH];
	int center_x, center_y;
	int height_diff, width_diff;
	int room_idx;
	cgc_size_t bytes;

	if(!(player = cgc_getObjectById(start, PLAYER_NUM)))
		_terminate(OBJECT_NOT_FOUND_ERROR);

	center_x = player->position->x;
	center_y = player->position->y;

	height_diff = (VIEW_HEIGHT - 1)/2;
	width_diff = (VIEW_WIDTH - 1)/2;

	room_idx = center_x / (ROOM_WIDTH - 1);

	int i;
	for(i=height_diff*(-1); i<=height_diff; i++) {
		cgc_bzero(view, VIEW_WIDTH);
		for(int j=width_diff*(-1); j<=width_diff; j++) {
			if(i+center_y < ROOM_HEIGHT && i+center_y >= 0) {

				if(j+center_x < room_idx*ROOM_WIDTH + ROOM_WIDTH &&
				   j+center_x >= room_idx*ROOM_WIDTH) {
				   	cgc_Room* room;
				   	if(!(room = cgc_getRoom(start, room_idx)))
				   		view[j+width_diff] = ' ';
				   	else
						view[j+width_diff] = room->contents[i+center_y][(j+center_x)%ROOM_WIDTH]->symbol;
				} else if(j+center_x >= room_idx*ROOM_WIDTH + ROOM_WIDTH) {
					cgc_Room* next_room;
					if(!(next_room = cgc_getRoom(start, room_idx+1)))
						view[j+width_diff] = ' ';
					else {
						view[j+width_diff] = next_room->contents[i+center_y][(j+center_x)%ROOM_WIDTH]->symbol;

					}
				} else if(j+center_x < room_idx*ROOM_WIDTH) {
					cgc_Room* prev_room;
					if(!(prev_room = cgc_getRoom(start, room_idx-1)))
						view[j+width_diff] = ' ';
					else {
						view[j+width_diff] = prev_room->contents[i+center_y][(j+center_x)%ROOM_WIDTH]->symbol;
					}
				}

			} else 
				view[j+width_diff] = ' ';
		}

#ifdef DEBUG
		if(cgc_transmit_all(STDOUT, &view, VIEW_WIDTH))
			_terminate(TRANSMIT_ERROR);


		if(cgc_transmit_all(STDOUT, "\n", cgc_strlen("\n")))
			_terminate(TRANSMIT_ERROR);
#endif
	}
}


/**
* Create room a room_string
* 
* @param room A pointer to the room array to hold the objects
* @param room_string A string representation of the room
* @param cornerStone The coordinates of the first object of the new room
* @param moveNum The number of moves the payer has made
*
* @return None
*/
void cgc_addRoom(cgc_Object* (*room)[ROOM_HEIGHT][ROOM_WIDTH], const char room_string[ROOM_HEIGHT][ROOM_WIDTH], cgc_Coordinate cornerStone, int moveNum) {

	for(int i=cornerStone.y; i<ROOM_HEIGHT; i++) {
		for(int j=cornerStone.x; j<(cornerStone.x + ROOM_WIDTH); j++) {
			switch(room_string[i%ROOM_HEIGHT][j%ROOM_WIDTH]) {
				case FLOOR_SYM:
					(*room)[i%ROOM_HEIGHT][j%ROOM_WIDTH] = cgc_makeObject(FLOOR_SYM, FLOOR_NUM, i, j, 0, 0, moveNum);
					break;
				case WALL_SYM:
					(*room)[i%ROOM_HEIGHT][j%ROOM_WIDTH] = cgc_makeObject(WALL_SYM, WALL_NUM, i, j, 0, 0, moveNum);
					break;
				case PLAYER_SYM:
					(*room)[i%ROOM_HEIGHT][j%ROOM_WIDTH] = cgc_makeObject(PLAYER_SYM, PLAYER_NUM, i, j, DIR_DOWN, 0, moveNum);
					break;
				case TREASURE_SYM:
					(*room)[i%ROOM_HEIGHT][j%ROOM_WIDTH] = cgc_makeObject(TREASURE_SYM, TREASURE_NUM, i, j, 0, 0, moveNum);
					break;
				case BAT_SYM: 
					(*room)[i%ROOM_HEIGHT][j%ROOM_WIDTH] = cgc_makeObject(BAT_SYM, BAT_NUM, i, j, DIR_UP, DIR_LEFT, moveNum);
					break;
				case ZOMBIE_SYM:
					(*room)[i%ROOM_HEIGHT][j%ROOM_WIDTH] = cgc_makeObject(ZOMBIE_SYM, ZOMBIE_NUM, i, j, DIR_DOWN, DIR_LEFT, moveNum);
					break;
				case EMPTY_SYM:
					(*room)[i%ROOM_HEIGHT][j%ROOM_WIDTH] = cgc_makeObject(EMPTY_SYM, EMPTY_NUM, i, j, 0, 0, moveNum);
					break;
				default:
					_terminate(INVALID_OBJ_SYMBOL);

			}

		}
	}
}

/**
* Destroy a room and cgc_free all memory
* 
* @param room A pointer to the room array to destroy
*
* @return None
*/
void cgc_destroyRoom(cgc_Object* (*room)[ROOM_HEIGHT][ROOM_WIDTH]) {

	for(int i=0; i<ROOM_HEIGHT; i++) {
		for(int j=0; j<ROOM_WIDTH; j++) {
			cgc_destroyObject((*room)[i][j]);
			(*room)[i][j] = 0;
		}
	}
}

/**
* Build the dungeon that the player will navigate
* 
* @param dungeon A pointer to the dungeon structure
*
* @return None
*/
void cgc_buildDungeon(cgc_Dungeon* dungeon) {
	char move_buf[10];
	cgc_Coordinate cornerStone;
	cornerStone.y = 0;
	cornerStone.x = 0;

	cgc_bzero(move_buf,10);
	if(!(dungeon->start = cgc_malloc(sizeof(cgc_Room))))
		_terminate(ALLOCATE_ERROR);

	cgc_addRoom(&dungeon->start->contents, first_room_string, cornerStone, 0);
	dungeon->start->next = NULL;

	dungeon->moveList = NULL;

	int in, im;
	unsigned char is_used['~'-'#'];
	cgc_bzero((char*)is_used, '~'-'#');

	im = 0;
	for (in = '~' - 10; in < '~' && im < 10; ++in) {
		char c = '\0';
		while(c < '#' || c > in) {
			if(cgc_random(&c, 1, 0))
				_terminate(ALLOCATE_ERROR);
		}

		if (is_used[c - '#'])
			c = in;

		move_buf[im++] = c;
		is_used[c - '#'] = 1;
	}

#if DEBUG
	dungeon->moveTypes.quit = 'Q';
	dungeon->moveTypes.left = 'L';
	dungeon->moveTypes.right = 'R';
	dungeon->moveTypes.jump = 'J';
	dungeon->moveTypes.jumpleft = 'H';
	dungeon->moveTypes.jumpright = 'K';
	dungeon->moveTypes.wait = 'W';
	dungeon->moveTypes.play = 'P';
	dungeon->moveTypes.instructions = 'I';
	dungeon->moveTypes.scores = 'S';
#else
	dungeon->moveTypes.quit = move_buf[0];
	dungeon->moveTypes.left = move_buf[1];
	dungeon->moveTypes.right = move_buf[2];
	dungeon->moveTypes.jump = move_buf[3];
	dungeon->moveTypes.jumpleft = move_buf[4];
	dungeon->moveTypes.jumpright = move_buf[5];
	dungeon->moveTypes.wait = move_buf[6];
	dungeon->moveTypes.play = move_buf[7];
	dungeon->moveTypes.instructions = move_buf[8];
	dungeon->moveTypes.scores = move_buf[9];
#endif

}

/**
* Destroy the dungeon and cgc_free all memory
* 
* @param dungeon A pointer to the dungeon structure
*
* @return None
*/
void cgc_destroyDungeon(cgc_Dungeon* dungeon) {
	cgc_size_t len;

	cgc_bzero((char *)&dungeon->moveTypes, sizeof(cgc_Moves));
	if(dungeon->moveList) {
		len = cgc_strlen(dungeon->moveList);
		cgc_bzero(dungeon->moveList, len);
		cgc_free(dungeon->moveList);		
	}
	dungeon->moveList = NULL;

	cgc_Room* nextRoom;
	for(cgc_Room* room=dungeon->start; room!=NULL; room=nextRoom) {
		nextRoom = (cgc_Room *)room->next;
		cgc_destroyRoom(&room->contents);
		cgc_free(room);
	}

}
