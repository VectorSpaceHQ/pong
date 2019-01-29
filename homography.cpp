#include <iostream>
#include <Eigen/Dense>

using namespace Eigen;
using namespace std;

int main(){
  /*
   Provide x,y coords for each of the four corners of the game area
   Provide the associated x,y positions from the galvonometer coord system
   Return a homography matrix, H, that is used to map from game to galvo coords
   */
  
  typedef Matrix<float, 9, 9> Matrix4f;
  typedef Matrix<float, 3, 1> Vector3f;


  // game coords, starting bottom left, go clockwise
  int32_t x1 = -143;
  int32_t x2 = -143;
  int32_t x3 = 150;
  int32_t x4 = 150;
  int32_t y1 = -64;
  int32_t y2 = 104;
  int32_t y3 = 104;
  int32_t y4 = -64;
               
  // galvo coords
  int32_t u1 = -500;
  int32_t u2 = -500;
  int32_t u3 = 500 ;
  int32_t u4 = 500 ;
  int32_t v1 = -500;
  int32_t v2 = 500 ;
  int32_t v3 = 500 ;
  int32_t v4 = -500;

  // int32_t x1 = settings.display.xMin;
  // int32_t x2 = x1;
  // int32_t x3 = settings.display.xMax;
  // int32_t x4 = x3;
  // int32_t y1 = settings.display.yMin;
  // int32_t y2 = settings.display.yMax;
  // int32_t y3 = y2;
  // int32_t y4 = settings.display.yMin;

  // // galvo coords
  // int32_t u1 = laserCal.botLeftX; 
  // int32_t u2 = laserCal.topLeftX;
  // int32_t u3 = laserCal.topRightX;
  // int32_t u4 = laserCal.botRightX;
  // int32_t v1 = laserCal.botLeftY;
  // int32_t v2 = laserCal.topLeftY;
  // int32_t v3 = laserCal.topRightY;
  // int32_t v4 = laserCal.botRightY;
  

  MatrixXd p(9,9);
  MatrixXd q(9,1);
  
  q(0,0) = 0;
  q(1,0) = 0;
  q(2,0) = 0;
  q(3,0) = 0;
  q(4,0) = 0;
  q(5,0) = 0;
  q(6,0) = 0;
  q(7,0) = 0;
  q(8,0) = 1;

  p(0,0) = -x1;
  p(0,1) = -y1;
  p(0,2) = -1;
  p(0,3) = 0;
  p(0,4) = 0;
  p(0,5) = 0;
  p(0,6) = x1*u1;
  p(0,7) = y1*u1;
  p(0,8) = u1;

  p(1,0) = 0;
  p(1,1) = 0;
  p(1,2) = 0;
  p(1,3) = -x1;
  p(1,4) = -y1;
  p(1,5) = -1;
  p(1,6) = x1*v1;
  p(1,7) = y1*v1;
  p(1,8) = v1;

  p(2,0) = -x2;
  p(2,1) = -y2;
  p(2,2) = -1;
  p(2,3) = 0;
  p(2,4) = 0;
  p(2,5) = 0;
  p(2,6) = x2*u2;
  p(2,7) = y2*u2;
  p(2,8) = u2;

  p(3,0) = 0;
  p(3,1) = 0;
  p(3,2) = 0;
  p(3,3) = -x2;
  p(3,4) = -y2;
  p(3,5) = -1;
  p(3,6) = x2*v2;
  p(3,7) = y2*v2;
  p(3,8) = v2;

  p(4,0) = -x3;
  p(4,1) = -y3;
  p(4,2) = -1;
  p(4,3) = 0;
  p(4,4) = 0;
  p(4,5) = 0;
  p(4,6) = x3*u3;
  p(4,7) = y3*u3;
  p(4,8) = u3;

  p(5,0) = 0;
  p(5,1) = 0;
  p(5,2) = 0;
  p(5,3) = -x3;
  p(5,4) = -y3;
  p(5,5) = -1;
  p(5,6) = x3*v3;
  p(5,7) = y3*v3;
  p(5,8) = v3;

  p(6,0) = -x4;
  p(6,1) = -y4;
  p(6,2) = -1;
  p(6,3) = 0;
  p(6,4) = 0;
  p(6,5) = 0;
  p(6,6) = x4*u4;
  p(6,7) = y4*u4;
  p(6,8) = u4;

  p(7,0) = 0;
  p(7,1) = 0;
  p(7,2) = 0;
  p(7,3) = -x4;
  p(7,4) = -y4;
  p(7,5) = -1;
  p(7,6) = x4*v4;
  p(7,7) = y4*v4;
  p(7,8) = v4;

  p(8,0) = 0;
  p(8,1) = 0;
  p(8,2) = 0;
  p(8,3) = 0;
  p(8,4) = 0;
  p(8,5) = 0;
  p(8,6) = 0;
  p(8,7) = 0;
  p(8,8) = 1;

  cout << "Here is the matrix q:\n" << q << endl;
  cout << "Here is the matrix p:\n" << p << endl;

  cout << "Here is the transpose of p\n" << p.transpose() << endl;  
  
  MatrixXd h;
  h = p.fullPivLu().solve(q);
  
  cout << "solved using LU decomposition with complete pivoting, h:\n" << h << endl;

  Map<MatrixXd> h_remap(h.data(), 3,3);

  MatrixXd H;
  H = h_remap.transpose();
    
  cout << "the homography matrix, H, is\n"<< H << endl;


  // test the result
  // ax=b
  // apply homography matrix to new set of projected positions
  // want laser to project to position, need to know where to set servos
  int x=30;
  int y=40;
  Vector3d a(x,y,1);

  cout << "The incoming x,y coordinates are:\n" << a << endl;
  cout << "The corrected x,y coordinates are:\n" << H * a << endl;
  
}

