#include "BodyMover.h"

void BodyMover::move(Body &body)
{
  legs.move(body.legs);
  waist.move(*body.waist);
  neck.move(body.neck);
}
