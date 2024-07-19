#include <dos.h>
#include <fs.h>
#define READ   0x2
#define WRITE  0x4
#define APPEND 0x8
#define BIN    0x0
#define PLUS   0x10
int fseek(FILE *fp, int offset, int whence) {
  if (whence == 0) {
    fp->p = offset;
  } else if (whence == 1) {
    fp->p += offset;
  } else if (whence == 2) {
    fp->p = fp->fileSize + offset;
  } else {
    return -1;
  }
  return 0;
}
long ftell(FILE *stream) {
  return stream->p;
}
#define CANREAD(flag)  ((flag)&READ || (flag)&PLUS)
#define CANWRITE(flag) ((flag)&WRITE || (flag)&PLUS || (flag)&APPEND)
FILE *fopen(char *filename, char *mode) {
  u32   flag = 0;
  FILE *fp   = (FILE *)malloc(sizeof(FILE));
  while (*mode != '\0') {
    switch (*mode) {
    case 'a': flag |= APPEND; break;
    case 'b': break;
    case 'r': flag |= READ; break;
    case 'w': flag |= WRITE; break;
    case '+': flag |= PLUS; break;
    default: break;
    }
    mode++;
  }
  if (vfs_filesize(filename) == -1) {
    free(fp);
    return NULL; // 找不到
  } else if (flag & WRITE) {
    char buffe2[100];
    // vfs_delfile(filename);
    // vfs_createfile(filename);
  }
  if (flag & WRITE) {
    fp->fileSize = 0;
  } else {
    fp->fileSize = vfs_filesize(filename);
  }
  fp->bufferSize = 0;
  if (flag & READ || flag & PLUS || flag & APPEND) {
    fp->bufferSize = vfs_filesize(filename);
    // printk("[Set]BufferSize=%d\n",fp->bufferSize);
  }
  if (flag & WRITE || flag & PLUS || flag & APPEND) { fp->bufferSize += 100; }
  if (fp->bufferSize == 0) { fp->bufferSize = 1; }
  fp->buffer = malloc(fp->bufferSize);
  if (flag & PLUS || flag & APPEND || flag & READ) {
    //	printk("ReadFile........\n");
    vfs_readfile(filename, fp->buffer);
  }
  fp->p = 0;
  if (flag & APPEND) { fp->p = fp->fileSize; }
  fp->name = malloc(strlen(filename) + 1);
  strcpy(fp->name, filename);
  fp->mode = flag;
  //	printk("[fopen]BufferSize=%d\n",fp->bufferSize);
  return fp;
}
int fgetc(FILE *stream) {
  if (CANREAD(stream->mode)) {
    if (stream->p >= stream->fileSize) {
      return EOF;
    } else {
      return stream->buffer[stream->p++];
    }
  } else {
    return EOF;
  }
}
int fputc(int ch, FILE *stream) {
  if (CANWRITE(stream->mode)) {
    //		printk("Current Buffer=%s\n",stream->buffer);
    if (stream->p >= stream->bufferSize) {
      //	printk("Realloc....(%d,%d)\n",stream->p,stream->bufferSize);
      stream->buffer      = realloc(stream->buffer, stream->bufferSize + 100);
      stream->bufferSize += 100;
    }
    if (stream->p >= stream->fileSize) { stream->fileSize++; }
    //		printk("Current Buffer=%s(A)\n",stream->buffer);
    stream->buffer[stream->p++] = ch;
    //	printk("Current Buffer=%s(B)\n",stream->buffer);
    return ch;
  }
  return EOF;
}
u32 fwrite(const void *ptr, u32 size, u32 nmemb, FILE *stream) {
  if (CANWRITE(stream->mode)) {
    u8 *c_ptr = (u8 *)ptr;
    for (int i = 0; i < size * nmemb; i++) {
      fputc(c_ptr[i], stream);
    }
    return nmemb;
  } else {
    return 0;
  }
}
u32 fread(void *buffer, u32 size, u32 count, FILE *stream) {
  if (CANREAD(stream->mode)) {
    u8 *c_ptr = (u8 *)buffer;
    for (int i = 0; i < size * count; i++) {
      u32 ch = fgetc(stream);
      if (ch == EOF) {
        return i;
      } else {
        c_ptr[i] = ch;
      }
    }
    return count;
  } else {
    return 0;
  }
}
int fclose(FILE *fp) {
  if (fp == NULL) { return EOF; }
  if (CANWRITE(fp->mode)) {
    //		printk("Save file.....(%s) Size =
    //%d\n",fp->buffer,fp->fileSize);
    //  Edit_File(fp->name, fp->buffer, fp->fileSize, 0);
    vfs_writefile(fp->name, fp->buffer, fp->fileSize);
  }
  free(fp->buffer);
  free(fp->name);
  free(fp);
  return 0;
}
char *fgets(char *str, int n, FILE *stream) {
  if (CANREAD(stream->mode)) {
    for (int i = 0; i < n; i++) {
      u32 ch = fgetc(stream);
      if (ch == EOF) {
        if (i == 0) {
          return NULL;
        } else {
          break;
        }
      }
      if (ch == '\n') { break; }
      str[i] = ch;
    }
    return str;
  }
  return NULL;
}
int fputs(const char *str, FILE *stream) {
  if (CANWRITE(stream->mode)) {
    for (int i = 0; i < strlen(str); i++) {
      fputc(str[i], stream);
    }
    return 0;
  }
  return EOF;
}
int fprintf(FILE *stream, const char *format, ...) {
  if (CANWRITE(stream->mode)) {
    int     len;
    va_list ap;
    va_start(ap, format);
    char *buf = malloc(1024);
    len       = vsprintf(buf, format, ap);
    fputs(buf, stream);
    free(buf);
    va_end(ap);
    return len;
  } else {
    // printk("CAN NOT WRITE\n");
    return EOF;
  }
}

int feof(FILE *stream) {
  if (stream->p >= stream->fileSize) { return EOF; }
  return 0;
}
int ferror(FILE *stream) {
  return 0;
}
int getc(FILE *stream) {
  return fgetc(stream);
}
int fsz(char *filename) {
  return vfs_filesize(filename);
}

void EDIT_FILE(char *name, char *dest, int length, int offset) {
  if (vfs_filesize(name) == -1) {
    //没有找到文件，创建一个，然后再编辑
    vfs_createfile(name);
    EDIT_FILE(name, dest, length, offset);
    return;
  }
  vfs_writefile(name, dest, length);
  return;
}
int Copy(char *path, char *path1) {
  u8 *path1_file_buffer;
  if (fsz(path) == -1) {
    // printk("file not found\n");
    return -1;
  }
  // printk("-----------------------------\n");
  vfs_createfile(path1);

  path1_file_buffer = kmalloc(fsz(path) + 1);
  int sz            = fsz(path);
  vfs_readfile(path, path1_file_buffer);
  vfs_writefile(path1, path1_file_buffer, sz);
  kfree(path1_file_buffer);
  return 0;
}