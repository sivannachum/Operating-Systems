# -*- coding: utf-8 -*-
"""PI / PythonThreads.ipynb

Automatically generated by Colaboratory.

Original file is located at
    https://colab.research.google.com/drive/1gCVpOaQ5zdqlQgQT8yJeu5HrIw7wqL0S

Professor's Work
"""

# Second Threading Example in the book (Fig 26.6; t1.c)
from threading import Thread, RLock
import threading
import sys
import time

counter = 0

# This may not have the result you expect because it does not use locking
def mythread(name):
  global counter
  sys.stdout.write("%s: begin\n" % name)
  for i in range(1_000_000):
    counter += 1
  sys.stdout.write("%s: done\n" % name)

p1 = Thread(target=mythread, args=("A"))
p2 = Thread(target=mythread, args=("B"))

start = time.time()
p1.start()
p2.start()
p1.join()
p2.join()
end = time.time()

sys.stdout.write("main: done with both (counter=%d) in %1.3fs\n" % (counter, end-start))

# Second Threading Example Fixed (but essentially worse than single-threaded)
# I toned "N" down because it takes so long to run!
from threading import Thread, Lock
import threading
import sys
import time

mutex = Lock()

counter = 0
def mythread(name):
  global counter
  sys.stdout.write("%s: begin\n" % name)
  for i in range(1_000_000):
    #mutex.acquire()
    #counter += 1
    #mutex.release()
    with mutex: # prefer with mutex syntax if you're going to use it so we release on exceptions/errors/returns etc.
      counter += 1
  sys.stdout.write("%s: done\n" % name)

p1 = Thread(target=mythread, args=["A"])
p2 = Thread(target=mythread, args=["B"])
start = time.time()
p1.start()
p2.start()
# wait for p1 to finish
p1.join()
# wait for p2 to finish
p2.join()
end = time.time()
sys.stdout.write("main: done with both (counter=%d) in %1.3fs\n" % (counter, end - start))

# Calculate PI: Sequential:

import random, time, math
N = 4_000_000 # number of x, y "darts"
yes = 0
start = time.time()

for _ in range(N):
  x = random.random()
  y = random.random()
  if x*x + y*y <= 1:
    yes += 1
end = time.time()

print('%1.9f' % ( (yes / N) * 4))
print(math.pi)
print("Time: %1.3fs" %  (end - start))

"""
Sample print:
3.142627000
3.141592653589793
Time: 1.459s
"""

# Calculate PI: Parallel

import random, time, math
from threading import Thread, RLock, Lock
import threading
import sys
# the lock
mutex = Lock()

overall_yes = 0
total = 0

# Estimate pi using "darts"
def estimate_pi_parts(N):
  # keep track of how many total darts hit in the unit circle
  global overall_yes
  # keep track of how many total darts are thrown
  # (hitting either inside or outside of the unit circle)
  global total
  
  # parallel part:
  # keep track of how many darts of yours have landed in the unit circle
  local_yes = 0
  # throw N darts
  for i in range(N):
    # get a random (x, y) in [0, 1)
    x = random.random()
    y = random.random()
    # check if that (x, y) are in the unit circle
    # if so, add to local_yes
    if x*x + y*y <= 1:
      local_yes += 1
      
  # critical section:
  # with the lock, update the overall_yes and the total
  with mutex:
    overall_yes += local_yes
    total += N

# track how long this all takes
start = time.time()

threads = []
N = 2_000_000 # number of (x, y) "darts" for each thread
NT = 4 # number of threads 

# create the threads and start them
for i in range(NT):
  t = Thread(target=estimate_pi_parts, args=[N])
  threads.append(t)
  t.start()
# wait for all the threads to finish:
for t in threads:
  t.join()

end = time.time()

# area of the unit circle is equal to PI*r*r = PI*1*1 = PI
# we are looking at the area of 1 quadrant out of 4
print('time: %1.3f seconds' % (end-start))
print('%1.9f' % (overall_yes / (total / 4)))

"""
Sample print:
time: 2.004 seconds
3.142118500
"""

"""
My Work
"""

# challenge: do rectangular integration (more stable pi estimate)
# Calculate PI: Parallel, using rectangular integration

import random, time, math
from threading import Thread, RLock, Lock
import threading
import sys
# the lock
mutex = Lock()

total = 0

# Estimate pi using rectangular integration
def estimate_pi_parts(N, start, end):
  # keep track of the total area of the rectangles
  global total
  
  # parallel part:
  # initialize x to the start
  x = start
  # have a variable to keep track of the width of the rectangles
  width = 1/N
  # keep track of your total area of rectangles calculated
  local_total = 0
  while (x < end):
    # calculate y
    y = math.sqrt(1-(x**2))
    # calculate the local total by adding the rectangle area to it
    local_total += y*width
    # go to the next rectangle starting point
    x += width
      
  # critical section:
  # with the lock, update the global total
  with mutex:
    total += local_total

# track how long this all takes
start = time.time()

threads = []
N = 2_000_000 # number of subdivision rectangles for each thread
NT = 4 # number of threads

# create the threads and start them
for i in range(NT):
  t = Thread(target=estimate_pi_parts, args=[N, i/NT, (i+1)/NT])
  threads.append(t)
  t.start()
# wait for all the threads to finish:
for t in threads:
  t.join()

end = time.time()

print('time: %1.3f seconds' % (end-start))
# just calculating the area of one quadrant of the unit circle, so times by 4
print('%1.9f' % (total*4))

"""
Sample print:
time: 0.429 seconds
3.141596913
"""

# challenge: do simpsons' rule (more stable pi estimate)
# https://youtu.be/ns3k-Lz7qWU
# integral_{a}^b(f(x)dx) =~ [(change_in_x)/3](f(x_0)+4f(x_1)+2f(x_2)+4(f_x3)+2f(x_4)+...+f(x_n))
# where change_in_x = (b-a)/n for some even n

# Calculate PI: Parallel, using Simpson's rule
import random, time, math
from threading import Thread, RLock, Lock
import threading
import sys
# the lock
mutex = Lock()

total = 0

# Estimate pi using simpson's rule
def estimate_pi_parts(N, start, end):
  # keep track of the total across all threads
  global total
  
  # parallel part:
  # initialize x to the start
  x = start
  # have a variable to keep track of the change in x
  change_in_x = (end-start)/N
  local_total = 0
  # calculate the first f(x) and add it to the local_total
  local_total += math.sqrt(1-(x**2))
  x += change_in_x
  # keep track of whether we need to times f(x) by 4 or by 2
  times_four = True
  last_while = end-change_in_x
  while (x < last_while):
    # calculate y
    y = math.sqrt(1-(x**2))

    # calculate the local_total by adding either 4f(x) or 2f(x)
    if (times_four):
        local_total += 4*y
        times_four = False
    else:
        local_total += 2*y
        times_four = True

    x += change_in_x
  
  # Now add the last f(x)
  local_total += math.sqrt(1-(x**2))

  # critical section:
  # with the lock, update the global total, following simpson's rule
  with mutex:
    total += (local_total * change_in_x/3.0)

# keep track of how long this all takes
start = time.time()

threads = []
N = 2_000_000 # number of f(x)'s each thread should calculate
NT = 4 # number of threads

# create the threads and start them
for i in range(NT):
  t = Thread(target=estimate_pi_parts, args=[N, i/NT, (i+1)/NT])
  threads.append(t)
  t.start()
# wait for all the threads to finish:
for t in threads:
  t.join()

end = time.time()

print('time: %1.3f seconds' % (end-start))
# just calculating the area of one quadrant of the unit circle, so times by 4
print('%1.9f' % (total*4))

"""
Sample print:
time: 2.069 seconds
3.141591635
"""

# challenge: if you know numpy, make dart throwing more effective

# Calculate PI: Parallel, with darts and numpy
import random, time, math
import numpy as np
from threading import Thread, RLock, Lock
import threading
import sys
# the lock
mutex = Lock()

overall_yes = 0
total = 0

# Estimate pi using "darts" AND numpy
def estimate_pi_parts(N):
  # keep track of how many total darts hit in the unit circle
  global overall_yes
  # keep track of how many total darts are thrown
  # (hitting either inside or outside of the unit circle)
  global total
  
  # parallel part:
  local_yes = 0
  # Calculate random arrays of x and y values in [0, 1)
  array_x = np.random.rand(N)
  array_y = np.random.rand(N)
  # square the elements in the arrays
  array_x_2 = np.square(array_x)
  array_y_2 = np.square(array_y)
  # check if an aligned (x, y) pair are in the unit circle
  # True = 1, False = 0 in the boolean array
  array_bool = ((array_x_2 + array_y_2) <= 1)
  # calculate how many points were in the unit circle
  # and add that total to local_yes
  local_yes += np.sum(array_bool)
      
  # critical section:
  # with the lock, update the overall_yes and the total
  with mutex:
    overall_yes += local_yes
    total += N

# keep track of how long this all takes 
start = time.time()

threads = []
N = 2_000_000 # number of (x, y) "darts" for each thread
NT = 4 # number of threads 

# create the threads and start them
for i in range(NT):
  t = Thread(target=estimate_pi_parts, args=[N])
  threads.append(t)
  t.start()
# wait for all the threads to finish:
for t in threads:
  t.join()

end = time.time()

print('time: %1.3f seconds' % (end-start))
# area of the unit circle is equal to PI*r*r = PI*1*1 = PI
# we are looking at the area of 1 quadrant out of 4
print('%1.9f' % (overall_yes / (total / 4)))
"""
Sample print:
time: 0.259 seconds
3.140549000
"""

"""
Analysis:
Throwing darts in Java on MY machine (HP Pavilion Laptop) is about as fast throwing darts in Python without numpy on GOOGLE's machine.
Throwing darts with numpy makes the threads ridiculously fast.
Rectangular integration is just a bit slower than numpy darts, but gives a more stable pi estimate (does not vary with runs).
Simpson's rule takes about as long as darts without numpy but gives a better and more stable estimate. 
However, Simpson's rule does not provide a better estimate than rectangular integration, which runs much faster.
"""