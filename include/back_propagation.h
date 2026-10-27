#ifndef BACK_PROPAGATION_H
#define BACK_PROPAGATION_H

#include "ia1.h"
#include "forward.h"
#include "math.h"

void back_propagation(struct resaux *resaux, const double *target);
char gessed_letter(const struct resaux *r);
double mse(const struct resaux *r, const double *target);
void update_weights(struct resaux *resaux, double lr);


#endif