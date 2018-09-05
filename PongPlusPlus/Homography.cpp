#include <MatrixMath.h>

void calcHomographyMatrix(){
  // game coords, starting bottom left, go clockwise
  int x1 = displayMin.x;
  int x2 = x1;
  int x3 = displayMax.x;
  int x4 = x3;
  int y1 = displayMin.y;
  int y2 = displayMax.y;
  int y3 = y2;
  int y4 = displayMin.y;

  // servo coords
  int u1 = botLeftX;
  int u2 = topLeftX;
  int u3 = topRightX;
  int u4 = botRightX;
  int v1 = botLeftY;
  int v2 = topLeftY;
  int v3 = topRightY;
  int v4 = botRightY;

  mtx_type p[9][9];
  mtx_type q[9];
  mtx_type h[9];

  q[0][0] = 0;
  q[1][0] = 0;
  q[2][0] = 0;
  q[3][0] = 0;
  q[4][0] = 0;
  q[5][0] = 0;
  q[6][0] = 0;
  q[7][0] = 0;
  q[8][0] = 1;

  p[0][0] = -x1;
  p[0][1] = -y1;
  p[0][2] = -1;
  p[0][3] = 0;
  p[0][4] = 0;
  p[0][5] = 0;
  p[0][6] = x1*u1;
  p[0][7] = y1*u1;
  p[0][8] = u1;

  p[1][0] = 0;
  p[1][1] = 0;
  p[1][2] = 0;
  p[1][3] = -x1;
  p[1][4] = -y1;
  p[1][5] = -1;
  p[1][6] = x1*v1;
  p[1][7] = y1*v1;
  p[1][8] = v1;

  p[2][0] = -x2;
  p[2][1] = -y2;
  p[2][2] = -1;
  p[2][3] = 0;
  p[2][4] = 0;
  p[2][5] = 0;
  p[2][6] = x2*u2;
  p[2][7] = y2*u2;
  p[2][8] = u2;

  p[3][0] = 0;
  p[3][1] = 0;
  p[3][2] = 0;
  p[3][3] = -x2;
  p[3][4] = -y2;
  p[3][5] = -1;
  p[3][6] = x2*v2;
  p[3][7] = y2*v2;
  p[3][8] = v2;

  p[4][0] = -x3;
  p[4][1] = -y3;
  p[4][2] = -1;
  p[4][3] = 0;
  p[4][4] = 0;
  p[4][5] = 0;
  p[4][6] = x3*u3;
  p[4][7] = y3*u3;
  p[4][8] = u3;

  p[5][0] = 0;
  p[5][1] = 0;
  p[5][2] = 0;
  p[5][3] = -x3;
  p[5][4] = -y3;
  p[5][5] = -1;
  p[5][6] = x3*v3;
  p[5][7] = y3*v3;
  p[5][8] = v3;

  p[6][0] = -x4;
  p[6][1] = -y4;
  p[6][2] = -1;
  p[6][3] = 0;
  p[6][4] = 0;
  p[6][5] = 0;
  p[6][6] = x4*u4;
  p[6][7] = y4*u4;
  p[6][8] = u4;

  p[7][0] = 0;
  p[7][1] = 0;
  p[7][2] = 0;
  p[7][3] = -x4;
  p[7][4] = -y4;
  p[7][5] = -1;
  p[7][6] = x4*v4;
  p[7][7] = y4*v4;
  p[7][8] = v4;

  p[8][0] = 0;
  p[8][1] = 0;
  p[8][2] = 0;
  p[8][3] = 0;
  p[8][4] = 0;
  p[8][5] = 0;
  p[8][6] = 0;
  p[8][7] = 0;
  p[8][8] = 1;

  // Serial.print(p);
  Matrix.Print((mtx_type*)p, 9, 9, "P");

  // h = Matrix.invert(Matrix.transpose(p,9,9) * p) * (Matrix.transpose(p,9,9) * q);
}
