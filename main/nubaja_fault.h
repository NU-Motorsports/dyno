#ifndef NUBAJA_FAULT_H_
#define NUBAJA_FAULT_H_

typedef struct fault {
  int overcurrent_fault;
  int overtemp_fault;
  int overvolt_fault;
  int trip;
} fault_t;

void clear_faults(fault_t *fault);

void print_faults(fault_t *fault);

#endif
