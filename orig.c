typedef unsigned int uuint;
struct x {
  int a;
  int b;
  int c;
};
int var1;
int f8(void) { return (0); }
int f0(int i) {
  if(i < 0)
    return i;
  if(i > 0)
    return i + i - 1;
  return i;
}
int f3(int i, int j, int x) { return (1); }
union a {
  int z;
};
int main(int argc, char *argv[]) {
  if(argc != 2)
    return -1;
  struct x x_;
  int a = atoi(argv[1]);
  printf("%d\n", f0(a));
  x_.a = 2;
  goto this_label;
this_label:
  return f0(a);
  int z;
}
