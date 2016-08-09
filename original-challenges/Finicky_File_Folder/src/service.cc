/*
 * Copyright (c) 2016 Kaprica Security, Inc.
 *
 * Permission is hereby granted, cgc_free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 */

#include <stdint.h>
#include <cstdio.h>
#include <cstdlib.h>
#include <cstring.h>
#ifdef DEBUG
#define dbg(s, ...) cgc_fprintf(stderr, "DEBUG:\t" s "\n", ##__VA_ARGS__)
#else
#define dbg(s, ...)
#endif
#define err(s, ...) ({ \
    cgc_fprintf(stderr, "ERROR:\t" s "\n", ##__VA_ARGS__); \
    cgc_exit(1); \
})
#define assert(__x) ({ \
    if (!(__x)) err("failed assertion"); \
})
#define min(__x, __y) (__x) < (__y) ? (__x) : (__y)
#define BUFFER_SZ 0x1000
typedef cgc_ssize_t cgc_off_t;
typedef cgc_uint32_t cgc_buffer_id_t;
typedef enum
{
  RDONLY = 1 << 0,
  RDWR   = 1 << 1,
  TRUNC  = 1 << 2,
  APPEND = 1 << 3,
} cgc_open_flags_t;
template<typename T>
class cgc_ArrayList
{
  private:
    static const cgc_size_t default_capacity_ = 1024;
    cgc_size_t capacity_;
    cgc_size_t length_;
    T** array_;
  public:
    cgc_ArrayList(void)
    {
      capacity_ = default_capacity_;
      length_ = 0;
      array_ = new T*[default_capacity_];
    }
    ~cgc_ArrayList(void)
    {
      delete array_;
    }

    int cgc_Append(T* value)
    {
      if (!value)
      {
        err("Appended nullptr");
      }

      array_[length_++] = value;

      if (length_ == capacity_)
      {
        capacity_ *= 2;
        array_ = static_cast<T**>(cgc_realloc(array_, sizeof(T *) * capacity_));
      }

      return 0;
    }

    T* cgc_Get(cgc_size_t index)
    {
      if (index >= length_)
      {
        err("Bad get: %d", (int)index);
      }

      return array_[index];
    }

    T* cgc_Last(void)
    {
      if (length_ < 1)
      {
        return nullptr;
      }

      return array_[length_ - 1];
    }

    T* cgc_RemoveByValue(T* elem)
    {
      for (cgc_size_t i = 0; i < length_; ++i)
      {
        if (array_[i] == elem)
        {
          return cgc_RemoveByIndex(i);
        }
      }

      return nullptr;
    }

    T* cgc_RemoveByIndex(cgc_size_t idx)
    {
      if (idx >= length_)
      {
        return nullptr;
      }

      T* removed = array_[idx];
      array_[idx] = nullptr;

      for (cgc_size_t i = idx; i < length_ - 1; ++i)
      {
        array_[i] = array_[i + 1];
      }

      array_[length_ -  1] = nullptr;
      length_--;

      return removed;
    }

    cgc_size_t cgc_Size()
    {
      return length_;
    }
};

class cgc_Buffer
{
  public:
    cgc_uint8_t data[BUFFER_SZ];
    static cgc_uint32_t counter;
    cgc_buffer_id_t id;
    cgc_uint32_t left_;
    bool free_;

    void cgc_MarkUsed(void);
    void cgc_MarkFree(void);

    cgc_Buffer(void)
    {
      id = counter++;
      left_ = BUFFER_SZ;
      free_ = true;
    }

    cgc_size_t cgc_Size(void)
    {
      return BUFFER_SZ - left_;
    }

    int cgc_SetContents(cgc_uint8_t* bytes, cgc_size_t offset, cgc_size_t len)
    {
      if (len > BUFFER_SZ - offset)
      {
        return -1;
      }

      cgc_memcpy(data + offset, bytes, len);
      if (BUFFER_SZ - offset - len < left_)
      {
        left_ = BUFFER_SZ - offset - len;
      }

      return 0;
    }
};

class cgc_BufferCache
{
#define NUM_BUFS 32
#define NUM_QS 32
  private:
    cgc_ArrayList<cgc_Buffer>* Queues_[NUM_QS];
    cgc_ArrayList<cgc_Buffer>* Free_;

  public:
    cgc_BufferCache(void)
    {
      for (cgc_size_t i = 0; i < NUM_QS; ++i)
      {
        Queues_[i] = new cgc_ArrayList<cgc_Buffer>();
      }

      Free_ = new cgc_ArrayList<cgc_Buffer>();
      cgc_AllocateMore();
    }

    cgc_Buffer* cgc_GetFree(void)
    {
      if (!Free_->cgc_Size())
      {
        cgc_AllocateMore();
      }

      cgc_Buffer* B = Free_->cgc_Get(0);
      Free_->cgc_RemoveByIndex(0);
      return B;
    }

    void cgc_AllocateMore(void)
    {
      for (cgc_size_t i = 0; i < NUM_BUFS; ++i)
      {
        Free_->cgc_Append(new cgc_Buffer());
      }
    }

    cgc_size_t cgc_FreeLeft(void)
    {
      return Free_->cgc_Size();
    }

    void cgc_AddFree(cgc_Buffer* B)
    {
      B->cgc_MarkFree();
      Free_->cgc_Append(B);
    }

    void cgc_MarkUsed(cgc_Buffer* B)
    {
      Free_->cgc_RemoveByValue(B);
      auto q = Queues_[B->id % NUM_QS];
      q->cgc_Append(B);
    }

    void cgc_MarkFree(cgc_Buffer* B)
    {
      return;
    }

    cgc_Buffer* cgc_Get(cgc_uint32_t id)
    {
      cgc_size_t idx = id % NUM_QS;
      auto q = Queues_[idx];
      for(cgc_size_t i = 0; i < q->cgc_Size(); ++i)
      {
        if (q->cgc_Get(i)->id == id)
        {
          return q->cgc_Get(i);
        }
      }

      return nullptr;
    }
};


cgc_uint32_t cgc_Buffer::counter = 0;
static cgc_BufferCache* BCache = new cgc_BufferCache();

void cgc_Buffer::cgc_MarkFree(void)
{
  free_ = true;
  left_ = BUFFER_SZ;
  BCache->cgc_MarkFree(this);
}

void cgc_Buffer::cgc_MarkUsed(void)
{
  free_ = false;
  BCache->cgc_MarkUsed(this);
}

class cgc_Chunk
{
  private:
    cgc_size_t len_;
    cgc_uint8_t* bytes_;
  public:
    cgc_Chunk(cgc_uint8_t* bytes, cgc_size_t len)
    {
      len_ = len;
      bytes_ = (cgc_uint8_t*)cgc_calloc(sizeof(cgc_uint8_t), len);
      cgc_memcpy(bytes_, bytes, len);
    }

    ~cgc_Chunk(void)
    {
      delete bytes_;
    }

    cgc_size_t cgc_Size(void)
    {
      return len_;
    }

    cgc_uint8_t* operator[](const cgc_size_t idx)
    {
      return bytes_ + idx;
    }
};

class cgc_BytesReader
{
  private:
    cgc_size_t index_;
    cgc_Chunk* chunk_;

  public:
    cgc_BytesReader(cgc_uint8_t* bytes, cgc_size_t len)
    {
      chunk_ = new cgc_Chunk(bytes, len);
      index_ = 0;
    }

    ~cgc_BytesReader(void)
    {
      delete chunk_;
    }

    cgc_size_t cgc_Size(void)
    {
      return chunk_->cgc_Size() - index_;;
    }

    bool cgc_Empty(void)
    {
      return index_ >= chunk_->cgc_Size();
    }

    cgc_uint8_t cgc_Read(void)
    {
      if (index_ >= chunk_->cgc_Size())
      {
        return 0;
      }

      return *(*chunk_)[index_++];
    }

    cgc_uint8_t* cgc_ReadN(cgc_size_t len, bool is_str = false)
    {
      if (index_ + len > chunk_->cgc_Size())
      {
        return nullptr;
      }

      cgc_uint8_t* ret = nullptr;

      if (!is_str)
      {
        ret = (*chunk_)[index_];
      }
      else
      {
        ret = new cgc_uint8_t(len + 1);
        cgc_memcpy(ret, (*chunk_)[index_], len);
        ret[len] = '\x00';
      }

      index_ += len;

      return ret;
    }
};

class cgc_Response
{
#define MAX_RESP_SZ (32 * 1024)
  private:
    enum {
      STATUS_OK = 1,
      STATUS_ERR,
    };

  public:
    int Status;
    cgc_size_t Length;
    cgc_uint8_t Bytes[MAX_RESP_SZ];

    cgc_Response(void)
    {
      Status = STATUS_ERR;
      Length = 0;
    }

    void cgc_SetOK(void)
    {
      Status = STATUS_OK;
    }

    int cgc_Append(cgc_uint8_t* bytes, cgc_size_t len)
    {
      if (Length + len >= MAX_RESP_SZ)
      {
        return -1;
      }

      cgc_memcpy(Bytes + Length, bytes, len);
      Length += len;

      return 0;
    }
};

class cgc_Request
{
  private:
    cgc_uint32_t f_num_;
    cgc_uint32_t size_;
    cgc_uint8_t* data_;

  public:
    cgc_Request(cgc_uint8_t n, cgc_uint32_t size, cgc_uint8_t* data)
    {
      size_ = size;
      f_num_ = n;
      data_ = data;
      assert(data_);
    }

    cgc_uint8_t cgc_Num(void)
    {
      return f_num_;
    }

    cgc_uint32_t cgc_Size(void)
    {
      return size_;
    }

    cgc_uint8_t* cgc_Data(void)
    {
      return data_;
    }
};

template<typename T>
class cgc_Pair
{
  public:
    T fst;
    T snd;

    cgc_Pair(T fst, T snd) : fst(fst), snd(snd) {}
};


class cgc_Pathname
{
  private:
    static const cgc_uint8_t seperator = '/';

    cgc_size_t size_;
    cgc_uint8_t* data_;
    cgc_ArrayList<cgc_Pair<cgc_size_t>>* components_ = new cgc_ArrayList<cgc_Pair<cgc_size_t>>();


  public:
    cgc_Pathname(const char* pathname)
    {
      size_ = cgc_strlen((char *)pathname);
      cgc_size_t i;
      cgc_size_t start;

      data_ = new cgc_uint8_t[size_ + 1];
      cgc_memcpy(data_, pathname, size_);
      start = i = 0;
      for (;i < size_; ++i)
      {
        if (pathname[i] == seperator)
        {
          if (i > start)
          {
            components_->cgc_Append(new cgc_Pair<cgc_size_t>(start, i));
          }

          while (pathname[i] == seperator)
          {
            i++;
          }

          start = i;
        }
      }

      if (i == size_ && i > start)
      {
        components_->cgc_Append(new cgc_Pair<cgc_size_t>(start, i));
      }
    }

    const char* cgc_Get(cgc_size_t index)
    {
      cgc_Pair<cgc_size_t>* p =  components_->cgc_Get(index);
      char* cpy = new char[p->snd - p->fst + 1];
      cgc_memcpy(cpy, data_ + p->fst, p->snd - p->fst);
      cpy[p->snd - p->fst] = '\0';
      return cpy;
    }

    char* cgc_AsString(void)
    {
      char* s = new char[size_ + 1];
      cgc_memcpy(s, data_, size_);
      s[size_] = '\0';
      return s;
    }

    cgc_size_t cgc_Size()
    {
      return components_->cgc_Size();
    }
};

class cgc_File
{
  private:
    cgc_ArrayList<cgc_buffer_id_t> buffers_;
    cgc_size_t ref_cnt_;

  public:
    const char* name;
    cgc_File(const char* path)
    {
      name = path;
      ref_cnt_ = 0;
    }

    ~cgc_File(void)
    {
      for (cgc_size_t i = 0; i < buffers_.cgc_Size(); ++i)
      {
        auto A = buffers_.cgc_Get(i);
        auto B = BCache->cgc_Get(*A);
        BCache->cgc_AddFree(B);
      }
    }

    bool operator ==(const cgc_File &b) const
    {
      return cgc_strcmp(name, b.name) == 0;
    }

    virtual cgc_size_t cgc_Size(void)
    {
      cgc_size_t ret = 0;
      for (cgc_size_t i = 0; i < buffers_.cgc_Size(); ++i)
      {
        ret += BCache->cgc_Get(*buffers_.cgc_Get(i))->cgc_Size();
      }

      return ret;
    }

    int cgc_IncRef(void)
    {
      ref_cnt_++;
      return 0;
    }

    int cgc_DecRef(void)
    {
      ref_cnt_--;
      return 0;
    }

    virtual int cgc_RefCnt(void)
    {
      return ref_cnt_;
    }

    int cgc_Write(cgc_uint8_t* bytes, cgc_size_t offset, cgc_size_t len)
    {
      if (!bytes)
      {
        return -1;
      }

      cgc_size_t cnt = 0;
      cgc_size_t buf = 0;
      cgc_size_t cpy = 0;

      if (offset)
      {
        buf = offset / BUFFER_SZ;
      }

      cgc_size_t req_buffs = (len + (BUFFER_SZ - 1)) / BUFFER_SZ;
      cgc_size_t cur_buffs = buffers_.cgc_Size();
      cgc_size_t offset_in_bufs = offset / BUFFER_SZ;
      cgc_size_t n_new_buffs = 0;
      if (req_buffs + offset_in_bufs > cur_buffs)
      {
        n_new_buffs = req_buffs - cur_buffs + offset_in_bufs;
      }

      if (BCache->cgc_FreeLeft() < n_new_buffs)
      {
        BCache->cgc_AllocateMore();
      }

      cgc_Buffer* bufp;
      while (cnt < len)
      {
        if (buf >= buffers_.cgc_Size())
        {
          bufp = BCache->cgc_GetFree();
          bufp->cgc_MarkUsed();
          buffers_.cgc_Append(&bufp->id);
        }
        else
        {
          bufp = BCache->cgc_Get(*buffers_.cgc_Get(buf));
        }

        if (offset && buf == (offset / BUFFER_SZ))
        {
          cpy = min(len, BUFFER_SZ - (offset % BUFFER_SZ));
          if (bufp->cgc_SetContents(bytes + cnt, offset % BUFFER_SZ, cpy) != 0)
          {
            err("Bad buffer write");
          }
        }
        else
        {
          cpy = min((len - cnt), BUFFER_SZ);
          if (bufp->cgc_SetContents(bytes + cnt, 0, cpy) != 0)
          {
            err("Bad buffer write");
          }
        }

        cnt += cpy;
        buf++;
      }

      return 0;
    }

    int cgc_Read(cgc_uint8_t* bytes, cgc_size_t offset, cgc_size_t len)
    {
      if (!bytes || len + offset > cgc_Size())
      {
        return -1;
      }

      cgc_size_t cnt = 0;
      cgc_size_t buf = 0;
      cgc_size_t cpy = 0;

      if (offset)
      {
        buf = offset / BUFFER_SZ;
      }

      while (cnt < len)
      {
        auto buf_idx = buffers_.cgc_Get(buf);
        auto bufp = BCache->cgc_Get(*buf_idx);

        if (offset && buf == (offset / BUFFER_SZ))
        {
          cpy = min(len, BUFFER_SZ - (offset % BUFFER_SZ));
          cgc_memcpy(bytes + cnt, bufp->data + (offset % BUFFER_SZ), cpy);
        } else {
          cpy = min((len - cnt), BUFFER_SZ);
          cgc_memcpy(bytes + cnt, bufp->data, cpy);
        }

        cnt += cpy;
        buf++;
      }

      return 0;
    }
};

template<typename L, typename R>
class cgc_Either
{
  private:
    bool is_left_;
  public:
    L* l;
    R* r;

    cgc_Either(L* left = nullptr, R* right = nullptr)
    {
      if (left && right)
      {
        err("cgc_Either can't be both");
      }

      l = left;
      r = right;
      if (left)
      {
        is_left_ = true;
      }
      else
      {
        is_left_ = false;
      }
    }

    ~cgc_Either(void)
    {
      if (l)
      {
        delete l;
      }

      if (r)
      {
        delete r;
      }
    }

    bool cgc_IsLeft(void)
    {
      return is_left_;
    }
};

class cgc_DirTree
{
  public:
    cgc_ArrayList<cgc_Either<cgc_DirTree, cgc_File>> children;
    const char* name;

    cgc_DirTree(const char* n)
    {
      name = n;
    }

    cgc_Either<cgc_DirTree, cgc_File>* cgc_Search(const char* s)
    {
      for (cgc_size_t i = 0; i < children.cgc_Size(); i++)
      {
        auto E = children.cgc_Get(i);
        if (E->cgc_IsLeft())
        {
          if (cgc_strcmp(E->l->name, s) == 0)
          {
            return E;
          };
        }
        else
        {
          if (cgc_strcmp(E->r->name, s) == 0)
          {
            return E;
          }
        }
      }
      return nullptr;
    }
};



class cgc_FileHandle
{
  public:
    cgc_File& file_;
    cgc_off_t offset_;
    cgc_open_flags_t flags_;

  cgc_FileHandle(cgc_File& file, cgc_open_flags_t flags) : file_(file), flags_(flags)
  {
    file.cgc_IncRef();
    if (flags & APPEND)
    {
      offset_ = file_.cgc_Size();
    }
    else
    {
      offset_ = 0;
    }
  }

  ~cgc_FileHandle(void)
  {
    file_.cgc_DecRef();
  }

  int cgc_Read(cgc_uint8_t* bytes, cgc_size_t len)
  {
    if (file_.cgc_Read(bytes, offset_, len) != 0)
    {
      err("Bad fh read");
    }

    offset_ += len;
    return len;
  }

  cgc_size_t cgc_Size(void)
  {
    return file_.cgc_Size();
  }

  int cgc_Write(cgc_uint8_t* bytes, cgc_size_t len)
  {
    if (flags_ & RDONLY)
    {
      return -1;
    }

    if (file_.cgc_Write(bytes, offset_, len) != 0)
    {
      err("Bad fh write");
    }

    offset_ += len;
    return len;
  }
};

auto Root = cgc_DirTree(nullptr);

#define MAX_FDS 1024
cgc_FileHandle* UserFDs[MAX_FDS];

int cgc_api_open(const char* pathname, cgc_open_flags_t flags, cgc_uint32_t mode)
{
  dbg("Called open(%s, %x, %x)", pathname, flags, mode);
  int fd = -1;

  if (flags & RDONLY && flags & RDWR)
  {
    return -1;
  }

  if (flags & TRUNC && flags & APPEND)
  {
    return -1;
  }

  if (flags & RDONLY)
  {
    flags = (cgc_open_flags_t)(RDONLY | TRUNC);
  }

  if (!(flags & TRUNC || flags & APPEND))
  {
    flags = (cgc_open_flags_t)(flags | APPEND);
  }

  if (!(flags & RDONLY || flags & RDWR))
  {
    flags = (cgc_open_flags_t)(flags | RDONLY);
  }

  auto node = new cgc_Either<cgc_DirTree, cgc_File>(&Root, nullptr);
  auto pn = new cgc_Pathname(pathname);

  for (cgc_size_t i = 0; i < pn->cgc_Size(); ++i)
  {
    if (node && node->cgc_IsLeft())
    {
      node = node->l->cgc_Search(pn->cgc_Get(i));
    }
    else
    {
      goto done;
    }
  }

  if (!node || node->cgc_IsLeft() || cgc_strcmp(node->r->name, pn->cgc_Get(pn->cgc_Size() - 1)) != 0)
  {
    goto done;
  }

  cgc_size_t i;
  for (i = 0; i < MAX_FDS; ++i)
  {
    if (UserFDs[i] == nullptr)
    {
      break;
    }
  }

  if (i == MAX_FDS)
  {
    goto done;
  }
  UserFDs[i] = new cgc_FileHandle(*node->r, flags);
  fd = i;

done:
  return fd;
}

cgc_ssize_t cgc_api_read(int fd, cgc_uint8_t* buffer, cgc_size_t count)
{
  if (fd >= MAX_FDS)
  {
    return -1;
  }

  cgc_FileHandle* FH = UserFDs[fd];
  if (!FH)
  {
    return -1;
  }

  if (count > FH->cgc_Size() - FH->offset_)
  {
    return -1;
  }

  return FH->cgc_Read(buffer, count);
}


cgc_ssize_t cgc_api_write(int fd, cgc_uint8_t* buffer, cgc_size_t count)
{
  if (fd >= MAX_FDS)
  {
    return -1;
  }

  cgc_FileHandle* FH = UserFDs[fd];

  if (!FH)
  {
    return -1;
  }

  return FH->cgc_Write(buffer, count);
}

cgc_off_t cgc_api_lseek(int fd, cgc_off_t offset, int whence)
{
  dbg("Called lseek(%d, %u, %d)", fd, offset, whence);
  cgc_off_t position = -1;

#define SEEK_SET  0
#define SEEK_CUR  1
  if (fd >= MAX_FDS)
  {
    return -1;
  }

  cgc_FileHandle* FH = UserFDs[fd];

  if (!FH)
  {
    return -1;
  }

  if (whence == SEEK_SET)
  {
    if (offset >= FH->cgc_Size())
    {
      return -1;
    }

    FH->offset_ = offset;
    position = FH->offset_;
  }
  else if (whence == SEEK_CUR)
  {
    if (offset + FH->offset_ >= FH->cgc_Size())
    {
      return -1;
    }

    FH->offset_ += offset;
    position = FH->offset_;
  }

  return position;
}

int cgc_api_close(int fd)
{
  dbg("Called close(%d)", fd);
  int ret = 0;
  if (fd >= MAX_FDS)
  {
    return -1;
  }

  cgc_FileHandle* FH = UserFDs[fd];

  if (!FH)
  {
    return -1;
  }

  UserFDs[fd] = nullptr;
  delete FH;
  return ret;
}

#define MAKE_PARENTS (1 << 1)
int cgc_api_creat(const char* pathname, int mode)
{
  int ret = -1;

  if (pathname && pathname[0] != '/')
  {
    return -1;
  }

  dbg("Called create(%s, %d)", pathname, mode);
  auto pn = cgc_Pathname(pathname);
  auto node = new cgc_Either<cgc_DirTree, cgc_File>(&Root);
  cgc_File* f = nullptr;

  for (cgc_size_t i = 0; i < pn.cgc_Size() - 1; ++i)
  {
    auto x = pn.cgc_Get(i);
    if (node->cgc_IsLeft())
    {
      auto old = node;
      node = node->l->cgc_Search(x);
      if (!node)
      {
        node = old;
        if (mode & MAKE_PARENTS)
        {
          auto new_node = new cgc_Either<cgc_DirTree, cgc_File>(new cgc_DirTree(x));
          node->l->children.cgc_Append(new_node);
          node = new_node;
        }
        else
        {
          goto done;
        }
      }
    }
    else
    {
      goto done;
    }
  }

  if (!node->cgc_IsLeft())
  {
    return -1;
  }

  for (cgc_size_t i = 0; i < node->l->children.cgc_Size(); ++i)
  {
    auto name = pn.cgc_Get(pn.cgc_Size() -1);
    auto c = node->l->children.cgc_Get(i);
    if (c->cgc_IsLeft())
    {
      if (cgc_strcmp(c->l->name, name) == 0)
      {
        goto done;
      }
    }
    else
    {
      if (cgc_strcmp(c->r->name, name) == 0)
      {
        goto done;
      }
    }
  }

  f = new cgc_File(pn.cgc_Get(pn.cgc_Size() - 1));
  node->l->children.cgc_Append(new cgc_Either<cgc_DirTree, cgc_File>(nullptr, f));
  ret = 0;

done:
  return ret;
}

int cgc_api_unlink(const char* pathname)
{
  dbg("Called unlink(%s)", pathname);

  int ret = -1;

  cgc_DirTree* dir;
  auto pn = cgc_Pathname(pathname);
  auto node = new cgc_Either<cgc_DirTree, cgc_File>(&Root);
  cgc_File* f = nullptr;

  for (cgc_size_t i = 0; i < pn.cgc_Size(); ++i)
  {
    auto x = pn.cgc_Get(i);
    if (node->cgc_IsLeft())
    {
      dir = node->l;
      node = node->l->cgc_Search(x);
      if (!node)
      {
        goto done;
      }
    }
    else
    {
      goto done;
    }
  }

  if (!node->cgc_IsLeft())
  {
    f = node->r;
#ifdef PATCHED_1
    if (f->cgc_RefCnt() < 1)
#else
    if (f->cgc_RefCnt() <= 1)
#endif
    {
      auto x = dir->children.cgc_RemoveByValue(node);
      delete x;
      ret = 0;
    }
  }

done:
 return ret;
}

int cgc_load_req(cgc_FILE* fd, cgc_Request** req)
{
  cgc_uint32_t f_num;
  cgc_uint32_t size;
  *req = nullptr;

  if (cgc_fread(&f_num, sizeof(f_num), fd) != sizeof(f_num))
  {
    return -1;
  }

  // Check for done signal
  if (f_num == 0xb7eb7e)
  {
    return -1;
  }

  if (cgc_fread(&size, sizeof(size), fd) != sizeof(size))
  {
    return -1;
  }


  cgc_uint8_t* data = new cgc_uint8_t[size];

  if (cgc_fread(data, size, fd) != size)
  {
    return -1;
  }

  *req = new cgc_Request(f_num, size, data);

  return 0;
}


typedef enum {
  OPEN_F_NUM = 1,
  READ_F_NUM,
  WRITE_F_NUM,
  LSEEK_F_NUM,
  CLOSE_F_NUM,
  CREAT_F_NUM,
  UNLINK_F_NUM,
} cgc_FuncID;

cgc_Response* cgc_process_req(cgc_Request* req)
{
  cgc_Response* resp = nullptr;
  cgc_BytesReader* br = new cgc_BytesReader(req->cgc_Data(), req->cgc_Size());

  switch (req->cgc_Num())
  {
    case OPEN_F_NUM:
      {
        cgc_size_t pathlen = *(cgc_size_t*)br->cgc_ReadN(sizeof(pathlen));
        const char* pathname = (const char*)br->cgc_ReadN(pathlen, true);
        cgc_open_flags_t flags = *(cgc_open_flags_t*)br->cgc_ReadN(sizeof(flags));
        cgc_uint32_t mode = *(cgc_uint32_t*)br->cgc_ReadN(sizeof(mode));
        int fd = cgc_api_open(pathname, flags, mode);

        resp = new cgc_Response();
        if (fd >= 0)
        {
          resp->cgc_Append((cgc_uint8_t*)&fd, sizeof(fd));
          resp->cgc_SetOK();
        }

        break;
      }
    case READ_F_NUM:
      {
        int fd = *(int *)br->cgc_ReadN(sizeof(fd));
        cgc_size_t count = *(cgc_size_t *)br->cgc_ReadN(sizeof(count));

        cgc_uint8_t* buf = new cgc_uint8_t[count];
        cgc_ssize_t rx_cnt = cgc_api_read(fd, buf, count);

        resp = new cgc_Response();
        if (rx_cnt >= 0)
        {
          resp->cgc_Append((cgc_uint8_t*)&rx_cnt, sizeof(rx_cnt));
          resp->cgc_Append(buf, rx_cnt);
          resp->cgc_SetOK();
        }

        break;
      }
    case WRITE_F_NUM:
      {
        int fd = *(int *)br->cgc_ReadN(sizeof(fd));
        cgc_size_t count = *(cgc_size_t *)br->cgc_ReadN(sizeof(count));
        cgc_uint8_t* buf = (cgc_uint8_t *)br->cgc_ReadN(count);

        cgc_ssize_t tx_cnt = cgc_api_write(fd, buf, count);

        resp = new cgc_Response();

        if (tx_cnt >= 0)
        {
          resp->cgc_Append((cgc_uint8_t*)&tx_cnt, sizeof(tx_cnt));
          resp->cgc_SetOK();
        }

        break;
      }
    case LSEEK_F_NUM:
      {
        int fd = *(int *)br->cgc_ReadN(sizeof(fd));
        cgc_off_t offset = *(cgc_off_t *)br->cgc_ReadN(sizeof(offset));
        int whence = *(int *)br->cgc_ReadN(sizeof(whence));
        cgc_off_t noffset = cgc_api_lseek(fd, offset, whence);
        resp = new cgc_Response();
        if (noffset >= 0)
        {
          resp->cgc_Append((cgc_uint8_t *)&noffset, sizeof(noffset));
          resp->cgc_SetOK();
        }
        break;
      }
    case CLOSE_F_NUM:
      {
        int fd = *(int *)br->cgc_ReadN(sizeof(fd));
        int ret = cgc_api_close(fd);
        resp = new cgc_Response();
        if (ret == 0)
        {
          resp->cgc_Append((cgc_uint8_t*)&ret, sizeof(fd));
          resp->cgc_SetOK();
        }
        break;
      }
    case CREAT_F_NUM:
      {
        cgc_size_t pathlen = *(cgc_size_t*)br->cgc_ReadN(sizeof(pathlen));
        const char* pathname = (const char*)br->cgc_ReadN(pathlen, true);
        cgc_uint32_t mode = *(cgc_uint32_t *)br->cgc_ReadN(sizeof(mode));
        int fd = cgc_api_creat(pathname, mode);
        resp = new cgc_Response();
        if (fd >= 0)
        {
          resp->cgc_Append((cgc_uint8_t*)&fd, sizeof(fd));
          resp->cgc_SetOK();
        }
        break;
      }
    case UNLINK_F_NUM:
      {
        cgc_size_t pathlen = *(cgc_size_t*)br->cgc_ReadN(sizeof(pathlen));
        const char* pathname = (const char*)br->cgc_ReadN(pathlen, true);
        int ret = cgc_api_unlink(pathname);
        resp = new cgc_Response();
        if (ret == 0)
        {
          resp->cgc_Append((cgc_uint8_t*)&ret, sizeof(ret));
          resp->cgc_SetOK();
        }
        break;
      }
  }
  return resp;
}
void cgc_send_resp(cgc_FILE* fd, cgc_Response* resp)
{
  if (!fd || !resp)
  {
    err("Bad cgc_send_resp");
  }
  cgc_fwrite(&resp->Status, sizeof(resp->Status), stdout);
  cgc_fwrite(&resp->Length, sizeof(resp->Length), stdout);
  cgc_fwrite(resp->Bytes, resp->Length, stdout);
}
extern "C" int __attribute__((fastcall)) main(int secret_page_i, char *unused[])
{
    cgc_uint32_t volatile *secret_page = (cgc_uint32_t *)secret_page_i;
    cgc_uint64_t delim = 0x8442e492f255bf31;
    /* cgc_fxlat(stdin, "HASHTAGYOLOSWAG"); */
    /* cgc_fxlat(stdout, "HASHTAGYOLOSWAG"); */
    cgc_uint32_t sig = 0;
    for (cgc_size_t i = 0; i < 0x1000 / sizeof(cgc_uint32_t); i++)
    {
      sig ^= *(secret_page + i);
    }
    cgc_Request* cur_req;
    cgc_printf("FSAAS\n");
    cgc_printf("%x\n", sig);
    cgc_fbuffered(stdout, 1);
    while (1)
    {
      cgc_printf("QQQ\n");
      cgc_fflush(stdout);
      if (cgc_load_req(stdin, &cur_req) != 0)
      {
        break;
      }
      cgc_Response* resp = cgc_process_req(cur_req);
      if (!resp)
      {
        break;
      }
      cgc_send_resp(stdout, resp);
      cgc_fwrite(&delim, sizeof(delim), stdout);
    }
    cgc_fflush(stdout);
    return 0;
}
