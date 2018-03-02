
// Return the nth fibonnaci number
int fn(int num) {
  if (num <= 1) {
    return num;
  }
  return fn(num-1)+fn(num-2);
}
