// Function to set/clear individual bits in 2-byte int (e.g. the error flags variable)
void setBit(unsigned int* number, byte n, byte val) {
  *number = *number & ~(1 << n) | (val << n);
}

