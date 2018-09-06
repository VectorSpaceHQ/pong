import numpy as np

def gauss_jordan(m, eps = 1.0/(10**10)):
  """Puts given matrix (2D array) into the Reduced Row Echelon Form.
     Returns True if successful, False if 'm' is singular.
     NOTE: make sure all the matrix items support fractions! Int matrix will NOT work!
     Written by Jarno Elonen in April 2005, released into Public Domain"""
  (h, w) = (len(m), len(m[0]))
  for y in range(0,h):
    maxrow = y
    for y2 in range(y+1, h):    # Find max pivot
      if abs(m[y2][y]) > abs(m[maxrow][y]):
        maxrow = y2
    (m[y], m[maxrow]) = (m[maxrow], m[y])
    if abs(m[y][y]) <= eps:     # Singular?
      return False
    for y2 in range(y+1, h):    # Eliminate column y
      c = m[y2][y] / m[y][y]
      for x in range(y, w):
        m[y2][x] -= m[y][x] * c
  for y in range(h-1, 0-1, -1): # Backsubstitute
    c  = m[y][y]
    for y2 in range(0,y):
      for x in range(w-1, y-1, -1):
        m[y2][x] -=  m[y][x] * m[y2][y] / c
    m[y][y] /= c
    for x in range(h, w):       # Normalize row y
      m[y][x] /= c
  
  return True

def inv(M):
  """
  return the inv of the matrix M
  """
  #clone the matrix and append the identity matrix
  # [int(i==j) for j in range_M] is nothing but the i(th row of the identity matrix
  m2 = [row[:]+[int(i==j) for j in range(len(M) )] for i,row in enumerate(M) ]
  # extract the appended matrix (kind of m2[m:,...]
  return [row[len(M[0]):] for row in m2] if gauss_jordan(m2) else None


# game positions, start bottom left
x1 = -143
x2 = -143
x3 = 150
x4 = 150
y1 = -64
y2 = 104
y3 = 104
y4 = -64

# servo positions
u1=-500
u2=-500
u3=500
u4=500
v1=-500
v2=500
v3=500
v4=-500

q = np.matrix([[0],
              [0],
              [0],
              [0],
              [0],
              [0],
              [0],
              [0],
              [1]])

p = np.matrix([[-x1, -y1, -1, 0, 0, 0, x1*u1, y1*u1, u1],
               [0, 0, 0, -x1, -y1, -1, x1*v1, y1*v1, v1],
               [-x2, -y2, -1, 0, 0, 0, x2*u2, y2*u2, u2],
               [0, 0, 0, -x2, -y2, -1, x2*v2, y2*v2, v2],
               [-x3, -y3, -1, 0, 0, 0, x3*u3, y3*u3, u3],
               [0, 0, 0, -x3, -y3, -1, x3*v3, y3*v3, v3],
               [-x4, -y4, -1, 0, 0, 0, x4*u4, y4*u4, u4],
               [0, 0, 0, -x4, -y4, -1, x4*v4, y4*v4, v4],
               [0, 0, 0, 0, 0, 0, 0, 0, 1]])

print(p)
p_T = np.transpose(p)
print(p_T)

C = p_T * p
print("C")
print(C)

print("inverted C")
print(np.linalg.inv(C))

print("Gause jordan inverted C")
print(inv(C.tolist()))

D = p_T * q
print("D")
print(D)



# solve for homography matrix
h = np.linalg.solve(p, q)
print("numpy solution")
print(h)
print()

H = np.linalg.inv(np.transpose(p) * p) * (np.transpose(p) * q)
H = H.reshape(3,3)
print(H)



# h3 = np.matrix([[1.1, 0, 0],
#       [0, 1, 0],
#       [0, 0, 1]])

# ax=b
# apply homography matrix to new set of projected positions
# want laser to project to position, need to know where to set servos
print("")
x=30
y=40
a=np.matrix([[x],
             [y],
             [1]])
print(np.dot(H, a))



