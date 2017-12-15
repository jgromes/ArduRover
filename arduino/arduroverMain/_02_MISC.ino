// Function to set/clear individual bits in 2-byte int
void setBit(unsigned int* number, byte n, byte val) {
  *number = *number & ~(1 << n) | (val << n);
}

