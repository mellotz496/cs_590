int f0(int i) {
  if(i > 0)
    return i + i - 1;
  return i;
}
int main(int argc, char *argv[]) {
  if(argc != 2)
    return -1;
  int a = atoi(argv[1]);
  printf("%d\n", f0(a));
  return 0;
}
