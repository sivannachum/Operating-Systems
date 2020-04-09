# -*- coding: utf-8 -*-
"""Lab 8: Random and First-Fit Alloc.ipynb

Automatically generated by Colaboratory.

Original file is located at
    https://colab.research.google.com/drive/15cQbU5g_ZSkvRpu0q9a31LiBSL9q6Z7G
"""

from typing import List, Tuple, Set, Dict, Optional
import random

class Block(object):
  """This represents a linear slice of memory.

  |....|xxxxx|....|
       ^ start()
             ^ end()
       ~~~~~~~ length()  
  """
  def __init__(self, start: int, length: int, name="Unnamed", used=False):
    self._start = start
    assert(length > 0)
    self._length = length
    self.name = name
    self.used = used
  def start(self) -> int:
    return self._start
  def end(self) -> int:
    return self._length + self._start
  def length(self) -> int:
    return self._length
  def begin_end(self) -> Tuple[int,int]:
    return (self.start(), self.end())
  def __eq__(self, other):
    return self.start() == other.start() and self.end() == other.end() and self.name == other.name and self.used == other.used
  def __hash__(self):
    return hash(self.name) ^ hash(self.start()) ^ hash(self.end())
  def __str__(self):
    return 'Block {}@{} len={}'.format(self.name, self.start(), self.length())
  def __repr__(self):
    return 'Block(start={}, length={}, name={})'.format(self._start, self._length, repr(self.name))

class Simulation(object):
  """A Simulation of a memory allocator!"""

  def __init__(self, algorithm: str = "random", size: int = 1000):
    """Create a free_list of unused blocks and a used_list of used blocks!"""
    self.algorithm = algorithm
    self.size = size
    self.free_list = [Block(start=0, length=size, name="Mem")]
    self.used_list = []    

  def check(self):
    size = 0
    for b in self.used_list:
      assert b.used == True, "Blocks in used_list should be marked as used!"
      size += b.length()
    for b in self.free_list:
      assert b.used == False, "Blocks in free_list should be marked as NOT used!"
      size += b.length()
    assert size == self.size, "The sum of sizes of all blocks, used and unused, should never change!"

  def allocate(self, name: str, size: int) -> Optional[Block]:
    """Handle an allocation of a block of size, with a new name."""
    # create a list of candidate blocks that are free and fit size.
    fits_list = []
    for block in self.free_list:
      if block.length() >= size:
        fits_list.append(block);

    if len(fits_list) == 0:
      print("malloc FAIL for {} with size={}".format(name, size))
      return None
    else:
      chosen_block = None
      if self.algorithm == 'random':
        # choose a random block
        block_number = random.randint(0, len(fits_list)-1)
        chosen_block = fits_list[block_number]
      elif self.algorithm == "first":
        # choose a block by finding the earliest address that fits
        fits_list = sorted(fits_list, key=lambda b: b.start())
        chosen_block = fits_list[0]
      else:
        raise ValueError("Those two fits are enough for the lab.")

      self.free_list.remove(chosen_block) # remove this block from the free list
      # put the given information in the allocated block, and split it if contains more space than asked for
      if chosen_block.length() == size:
        block.name = name
        block.used = True
        self.used_list.append(chosen_block)
      else: # block is bigger than the size requested
        # split it into two:

        # First new block - the one to be used
        start: int = chosen_block.start()
        used = Block(start, size, name)
        used.used = True
        # Add this block to the used list
        self.used_list.append(used)

        # Second new block - the leftover unused space
        start = start + size
        length = chosen_block.end() - start
        leftover_unused = Block(start, length, chosen_block.name)
        # Add this block back to the free list
        self.free_list.append(leftover_unused)

  def free(self, name: str):
    """Free a block by name."""
    # Find the block to be freed
    to_free = None
    for b in self.used_list:
      if b.name == name:
        to_free = b
        break
    # Return if the user requested to free a block that is not in the used list
    if to_free is None:
      print("free FAIL; no such name={}", to_free)
      return
    # Remove the block from the used list
    to_free.used = False
    self.used_list.remove(to_free)
    
    # Check if either of the neighbors of this block are free and need to be coalesced with this block
    left_end_address = to_free.start()
    right_start_address = to_free.end()
    left_block = None
    right_block = None
    for block in self.free_list:
      if (block.end() == left_end_address):
        left_block = block
      elif (block.start() == right_start_address):
        right_block = block

    # Coalesce neighboring blocks if necessary, add blocks to free list
    if (left_block is not None and right_block is not None):
      # Remove these blocks so they can be coalesced with each other and with to_free
      self.free_list.remove(left_block)
      self.free_list.remove(right_block)
      start: int = left_block.start()
      end: int = right_block.end()
      new_name: str = left_block.name + name + right_block.name
      new_free = Block(start, end-start, new_name)
      self.free_list.append(new_free)
    elif (left_block is not None):
      # Remove this block so it can be coalesced with to_free
      self.free_list.remove(left_block)
      start: int = left_block.start()
      end: int = right_start_address
      new_name: str = left_block.name + name
      new_free = Block(start, end-start, new_name)
      self.free_list.append(new_free)
    elif (right_block is not None):
      # Remove this block so it can be coalesced with to_free
      self.free_list.remove(right_block)
      start: int = left_end_address
      end: int = right_block.end()
      new_name: str = name + right_block.name
      new_free = Block(start, end-start, new_name)
      self.free_list.append(new_free)
    else:
      # None of to_free's neighbors are free, so just add it alone to the free list
      self.free_list.append(to_free)

# See: 
# https://docs.google.com/presentation/d/12xHtwBlVBemDuKTQDv8j-AASzEAO9Ta3Qyx7dAldcmA/edit
# for a first-fit version of this simulation:

SIM_1_APRIL = """
pool random 1000
alloc A 200
alloc B 300
alloc C 100
free B
alloc D 150
alloc E 100
alloc F 200
free D
alloc G 100
alloc H 100
""".strip()

def run_simulation(input: str):
  assert input.count("pool") == 1, "Only 1 pool command allowed."
  commands = input.strip().splitlines()
  assert commands[0].startswith("pool"), "The pool command should be first."

  sim_args = commands[0].split()
  sim = Simulation(sim_args[1], int(sim_args[2]))

  for line in commands[1:]:
    # check simulation invariants
    sim.check()
    # execute next command
    words = line.split()
    if words[0] == 'alloc':
      sim.allocate(words[1], int(words[2]))
    elif words[0] == 'free':
      sim.free(words[1])
    else:
      raise ValueError(line)
    # check simulation invariants
    sim.check()
  return sim

output = run_simulation(SIM_1_APRIL)
print(output.free_list)
print(output.used_list)