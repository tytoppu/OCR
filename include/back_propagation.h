#ifndef BACK_PROPAGATION_H
#define BACK_PROPAGATION_H

#include "ai1.h"
#include "forward.h"
#include "math.h"

void back_propagation(struct network *network, const double *target);
char guessed_letter(const struct network *r);
double mse(const struct network *r, const double *target);
void update_weights(struct network *network, double lr);


#endif