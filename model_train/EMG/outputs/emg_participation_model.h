#ifndef EMG_PARTICIPATION_MODEL_H
#define EMG_PARTICIPATION_MODEL_H

#include <stdint.h>

typedef enum
{
  EMG_PARTICIPATION_RELAX = 0,
  EMG_PARTICIPATION_WEAK = 1,
  EMG_PARTICIPATION_ACTIVE = 2
} EmgParticipation_t;

typedef struct
{
  uint16_t raw;
  int32_t baseline_x10;
  int32_t drop_x10;
  int32_t rectified_x10;
  int32_t envelope_x10;
  int32_t rms_x10;
} EmgParticipationInput_t;

EmgParticipation_t EmgParticipationModel_Predict(const EmgParticipationInput_t *input, uint16_t *confidence_x1000);
const char *EmgParticipationModel_LabelName(EmgParticipation_t label);

#endif
