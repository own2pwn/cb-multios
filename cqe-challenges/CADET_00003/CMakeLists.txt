set( CC "clang")
set( CFLAGS " /Zi /WX   /wd4244 /wd4101 /wd4018 /wd4102 /wd4820 /wd4133 /wd4129")
set( INCLUDES "-Ilib")
buildCB(${CFLAGS})