#include "Sharpish.h"
#include "RFrame.h"

using namespace CS;

Float3 operator*(Float3 v, RFrame frame)
{
    return v.Rotate(frame.Rotation) + frame.Position;
}
