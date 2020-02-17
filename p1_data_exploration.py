from datetime import datetime
from typing import Optional, List, Set, Dict
import gzip
import random

DATA_URL = "https://static.jjfoley.me/os/2020_jobs.txt.gz"

# First 10 and last 10 lines of the above file!
EXAMPLE_LINES = """
 ReqCPUS     ReqMem      ReqGRES              Submit               Start                 End      State
-------- ---------- ------------ ------------------- ------------------- ------------------- ----------
       8    32000Mn        gpu:1 2020-01-01T00:44:28 2020-01-01T00:44:29 2020-01-01T07:33:15  COMPLETED
       8    32000Mn        gpu:1 2020-01-01T00:46:01 2020-01-01T00:46:01 2020-01-03T00:17:50  COMPLETED
       1    62.50Gn        gpu:1 2020-01-01T01:01:50 2020-01-01T01:02:00 2020-01-01T01:36:21  COMPLETED
       1    62.50Gn        gpu:1 2020-01-01T01:14:01 2020-01-01T01:14:02 2020-01-01T13:32:02  COMPLETED
       1    62.50Gn        gpu:1 2020-01-01T01:14:03 2020-01-01T01:14:05 2020-01-01T13:23:55  COMPLETED
       1    62.50Gn        gpu:1 2020-01-01T01:14:04 2020-01-01T01:14:05 2020-01-01T13:45:16  COMPLETED
       1    62.50Gn        gpu:1 2020-01-01T01:14:05 2020-01-01T01:14:05 2020-01-01T13:46:21  COMPLETED
       1    62.50Gn        gpu:1 2020-01-01T01:14:06 2020-01-01T01:14:08 2020-01-01T13:47:31  COMPLETED
...       
       1       50Gn        gpu:4 2020-01-31T11:35:38 2020-01-31T11:35:38 2020-01-31T11:48:36  COMPLETED
       1       50Gn        gpu:4 2020-01-31T11:36:05 2020-01-31T11:36:05 2020-01-31T11:48:43  COMPLETED
       1       32Gn        gpu:1 2020-01-31T11:36:14 2020-01-31T11:36:14 2020-01-31T11:39:09     FAILED
       1       32Gn        gpu:1 2020-01-31T11:39:11 2020-01-31T11:39:11 2020-01-31T11:39:49     FAILED
       1       32Gn        gpu:1 2020-01-31T11:39:50 2020-01-31T11:39:50 2020-01-31T11:47:23     FAILED
       1       50Gn        gpu:1 2020-01-31T11:39:56 2020-01-31T11:39:56             Unknown    RUNNING
       1       50Gn        gpu:4 2020-01-31T11:48:29 2020-01-31T11:48:29 2020-01-31T12:04:46     FAILED
       1       50Gn        gpu:4 2020-01-31T11:48:39 2020-01-31T11:48:39 2020-01-31T12:04:16     FAILED
       1       50Gn        gpu:4 2020-01-31T11:48:44 2020-01-31T11:48:44 2020-01-31T12:04:10     FAILED
       1      300Gn        gpu:8 2020-01-31T11:52:02 2020-01-31T11:52:02             Unknown    RUNNING
"""

class Job(object):
  """
  This class represents the state of a simulated job: 
  how long it needs to run for, its duration, when it started, etc.
  """
  def __init__(self, 
               arrival_time: float, 
               duration: float, 
               tickets: Optional[float] = None,
               start_time: Optional[float] = None, 
               run_time: Optional[float]=None,
               end_time: Optional[float]=None):
    self.arrival_time = arrival_time
    self.duration = duration
    self.tickets = tickets
    self.start_time = start_time
    self.run_time = run_time
    self.end_time = end_time
    self.last_run_time = 0

  def started(self) -> bool:
    """
    Has this job started yet?
    """
    return self.start_time is not None

  def finished(self) -> bool:
    """
    Has this job finished?
    """
    return self.end_time is not None
  
  def has_arrived(self, now: float) -> bool:
    """
    Given time=now, has this job "arrived" yet?
    """
    return self.arrival_time <= now

  def run(self, now: float, steps: float = 1):
    """
    Run this job for a little bit.

    Parameters
      now: what time is it now? If we're starting a job we need to save the time.
      steps: how long to run it for, e.g., 1 step, the job's duration, etc.
    """
    if self.finished():
      # crash if already finished.
      raise ValueError("Don't run a job that's finished!")

    # start the job if not started!
    if not self.started():
      self.start_time = now
      self.run_time = 0
    # Move it forward by steps amount of work.
    self.run_time += steps
    # Record last runtime as now + steps so no issue if now is 0
    self.last_run_time = now + steps
    # If that makes it done, deal with that:
    if self.run_time >= self.duration:
      # We might have gone over the needed amount of time.
      extra_time = self.run_time - self.duration
      used_time = steps - extra_time
      # record end time
      self.end_time = now + used_time

"""
Here we are just looking at the EXAMPLE_LINES
"""
# Keep track of the jobs
jobs: List[Job] = []
# Base arrival_time in Job objects based on when first job arrived
first_job_arrival: datetime = None

# Keep track of how many jobs we have seen and 
# the sums of their response times and turnaround times for the average
response_time_count = 0
response_time_sum = 0
turnaround_time_count = 0
turnaround_time_sum = 0

# Initiliaze min and max variables for response time and turnaround time
response_time_min = 9999999999999
response_time_max = 0
turnaround_time_min = 9999999999999
turnaround_time_max = 0

# These values are specifically for response_time only considering completed jobs
response_time_sum_completed = 0
response_time_min_completed = 9999999999999
response_time_max_completed = 0

# Split the data in EXAMPLE_LINES based on line breaks
lines = EXAMPLE_LINES.split('\n')
for line in lines:
  # Split each line based on whitespace
  values = line.split()
  # Don't crash if there aren't enough values!
  if (len(values) > 6):
    # Set the values that will be necessary for the calculations and Job object initialization
    submit, start, end, state = values[3], values[4], values[5], values[6]
    # Make sure we're actually dealing with a job and not just whitespace or headers
    if state == 'COMPLETED':
      # Utilized this resource for help with the strptime method
      # https://www.journaldev.com/23365/python-string-to-datetime-strptime
      submit_datetime_object = datetime.strptime(submit, '%Y-%m-%dT%H:%M:%S')
      start_datetime_object = datetime.strptime(start, '%Y-%m-%dT%H:%M:%S')
      end_datetime_object = datetime.strptime(end, '%Y-%m-%dT%H:%M:%S')

      # Utilized this resource to figure out how to subtract two datetimes
      # https://stackoverflow.com/questions/32211596/subtract-two-datetime-objects-python

      # Calculate response time and turnaround time for this job
      # response_time = start - submit
      response_time: float = (start_datetime_object - submit_datetime_object).total_seconds()
      # turnaround_time = end - submit
      turnaround_time: float = (end_datetime_object - submit_datetime_object).total_seconds()

      # Initialize first_job_arrival
      if (first_job_arrival is None):
        first_job_arrival = submit_datetime_object

      # Create a Job object from this job to test it later with a different scheduler
      arrival_time: float = (submit_datetime_object - first_job_arrival).total_seconds()
      duration: float = (end_datetime_object - start_datetime_object).total_seconds()
      job: Job = Job(arrival_time, duration)
      # Add this job to our list of jobs
      jobs.append(job)

      # Other book-keeping
      # Stuff for average calculations at end
      response_time_count += 1
      response_time_sum += response_time
      response_time_sum_completed += response_time
      turnaround_time_count += 1
      turnaround_time_sum += turnaround_time
      
      # Checking if new response time max or min
      if response_time > response_time_max:
        response_time_max = response_time
      elif response_time < response_time_min:
        response_time_min = response_time

      # Checking if new response time max or min just among completed jobs
      if response_time > response_time_max_completed:
        response_time_max_completed = response_time
      elif response_time < response_time_min_completed:
        response_time_min_completed = response_time

      # Checking if new turnaround time max or min
      if turnaround_time > turnaround_time_max:
        turnaround_time_max = turnaround_time
      elif turnaround_time < turnaround_time_min:
        turnaround_time_min = turnaround_time

    # Also count running and failed jobs for the average response time
    elif state == 'RUNNING' or state == 'FAILED':
      submit_datetime_object = datetime.strptime(submit, '%Y-%m-%dT%H:%M:%S')
      start_datetime_object = datetime.strptime(start, '%Y-%m-%dT%H:%M:%S')

      # Calculate response time for this job
      # response_time = start - submit
      response_time: float = (start_datetime_object - submit_datetime_object).total_seconds()

      # Other book-keeping
      # Stuff for average calculation at end
      response_time_count += 1
      response_time_sum += response_time
      
      # Checking if new response time max or min
      if response_time > response_time_max:
        response_time_max = response_time
      elif response_time < response_time_min:
        response_time_min = response_time

# Calculate the average response time and average turnaround time
average_response_time = response_time_sum/response_time_count
average_response_time_completed = response_time_sum_completed/len(jobs)
average_turnaround_time = turnaround_time_sum/turnaround_time_count

print("----------EXAMPLE_LINES----------")
print("Completed, running, and failed jobs:")
print("Average response time:", average_response_time, 
      "seconds\nMin response time:", response_time_min, 
      "seconds\nMax response time:", response_time_max, "seconds\n")

print("\nCompleted jobs only:")
print("Average response time:", average_response_time_completed, 
      "seconds\nMin response time:", response_time_min_completed, 
      "seconds\nMax response time:", response_time_max_completed, "seconds\n")

print("Average turnaround time:", average_turnaround_time/(60*60), 
      "hours\nMin turnaround time:", turnaround_time_min, 
      "seconds\nMax turnaround time:", turnaround_time_max/(60*60), "hours\n")


"""
NOW LOOKING AT ALL OF THE DATA
"""
# Keep track of the jobs
jobs: List[Job] = []
# Base arrival_time in Job objects based on when first job arrived
first_job_arrival: datetime = None
# List of end times to evaluate fairness
fairness_eval: List[float] = []

# Keep track of how many jobs we have seen and 
# the sums of their response times and turnaround times for the average
response_time_count = 0
response_time_sum = 0
turnaround_time_count = 0
turnaround_time_sum = 0

# Initiliaze min and max variables for response time and turnaround time
response_time_min = 9999999999999
response_time_max = 0
turnaround_time_min = 9999999999999
turnaround_time_max = 0

# These values are specifically for response_time only considering completed jobs
response_time_sum_completed = 0
response_time_min_completed = 9999999999999
response_time_max_completed = 0

# Utilized this resource for help with opening a zip file in Python:
# https://cmdlinetips.com/2018/02/how-to-read-a-gzip-file-in-python/
with gzip.open('2020_jobs.txt.gz', 'rb') as f:
  for line in f:
    # Split each line based on whitespace
    values = line.split()
    # Don't crash if there aren't enough values!
    if (len(values) > 6):
      # Set the values that will be necessary for the calculations and Job object initialization
      # Utilized this resource for help with figuring out how to get byte like objects to strings
      # https://stackoverflow.com/questions/606191/convert-bytes-to-a-string
      submit, start, end, state = values[3].decode("utf-8"), values[4].decode("utf-8"), values[5].decode("utf-8"), values[6].decode("utf-8")
      # Make sure we're actually dealing with a job and not just whitespace or headers
      if state == 'COMPLETED':
        # Utilized this resource for help with the strptime method
        # https://www.journaldev.com/23365/python-string-to-datetime-strptime
        submit_datetime_object = datetime.strptime(submit, '%Y-%m-%dT%H:%M:%S')
        start_datetime_object = datetime.strptime(start, '%Y-%m-%dT%H:%M:%S')
        end_datetime_object = datetime.strptime(end, '%Y-%m-%dT%H:%M:%S')

        # Utilized this resource to figure out how to subtract two datetimes
        # https://stackoverflow.com/questions/32211596/subtract-two-datetime-objects-python

        # Calculate response time and turnaround time for this job
        # response_time = start - submit
        response_time: float = (start_datetime_object - submit_datetime_object).total_seconds()
        # turnaround_time = end - submit
        turnaround_time: float = (end_datetime_object - submit_datetime_object).total_seconds()

        # Initialize first_job_arrival
        if (first_job_arrival is None):
          first_job_arrival = submit_datetime_object

        """
        With the lottery scheduler, I predicted that by alloting tickets well,
        I would improve the average turnaround time.
        Given that the average job duration is ~4600 (calculations later in the code),
        I divided a job's duration by 300 to determine how many tickets it should get.
        On a different run, I simply gave each job one ticket.
        """
        # Create a Job object from this job to test it later with a different scheduler
        arrival_time: float = (submit_datetime_object - first_job_arrival).total_seconds()
        duration: float = (end_datetime_object - start_datetime_object).total_seconds()
        tickets: int = (duration//300)
        if (tickets == 0):
          tickets = 1
        #tickets = 1
        job: Job = Job(arrival_time, duration, tickets)
        # Add this job to our list of jobs
        jobs.append(job)

        end_time = (end_datetime_object - first_job_arrival).total_seconds()
        fairness_eval.append(end_time)

        # Other book-keeping
        # Stuff for average calculations at end
        response_time_count += 1
        response_time_sum += response_time
        response_time_sum_completed += response_time
        turnaround_time_count += 1
        turnaround_time_sum += turnaround_time
      
        # Checking if new response time max or min
        if response_time > response_time_max:
          response_time_max = response_time
        elif response_time < response_time_min:
          response_time_min = response_time

        # Checking if new response time max or min just among completed jobs
        if response_time > response_time_max_completed:
          response_time_max_completed = response_time
        elif response_time < response_time_min_completed:
          response_time_min_completed = response_time

        # Checking if new turnaround time max or min
        if turnaround_time > turnaround_time_max:
          turnaround_time_max = turnaround_time
        elif turnaround_time < turnaround_time_min:
          turnaround_time_min = turnaround_time

      # Also count running and failed jobs for the average response time
      elif state == 'RUNNING' or state == 'FAILED':
        submit_datetime_object = datetime.strptime(submit, '%Y-%m-%dT%H:%M:%S')
        start_datetime_object = datetime.strptime(start, '%Y-%m-%dT%H:%M:%S')

        # Calculate response time for this job
        # response_time = start - submit
        response_time: float = (start_datetime_object - submit_datetime_object).total_seconds()

        # Other book-keeping
        # Stuff for average calculation at end
        response_time_count += 1
        response_time_sum += response_time
      
        # Checking if new response time max or min
        if response_time > response_time_max:
          response_time_max = response_time
        elif response_time < response_time_min:
          response_time_min = response_time

# Calculate the average response time and average turnaround time
average_response_time = response_time_sum/response_time_count
average_response_time_completed = response_time_sum_completed/len(jobs)
average_turnaround_time = turnaround_time_sum/turnaround_time_count
# Calculate fairness
fairness_eval.sort()
sum = 0
# Compare each job to its neighbor
n: int = len(fairness_eval)-1
for i in range(n):
  sum += fairness_eval[i]/fairness_eval[i+1]
average_fairness = sum/n

print("\n----------THE ENTIRE FILE----------")
print("Completed, running, and failed jobs:")
print("Average response time:", average_response_time, 
      "seconds\nMin response time:", response_time_min, 
      "seconds\nMax response time:", response_time_max, "seconds\n")

print("\nCompleted jobs only:")
print("Average response time:", average_response_time_completed, 
      "seconds\nMin response time:", response_time_min_completed, 
      "seconds\nMax response time:", response_time_max_completed, "seconds\n")

print("Average turnaround time:", average_turnaround_time/(60*60), 
      "hours\nMin turnaround time:", turnaround_time_min, 
      "seconds\nMax turnaround time:", turnaround_time_max/(60*60), "hours\n")

print("Fairness:", average_fairness)

# This is where I calculated the average job duration
# Minimum job duration = 0
# Average job duration = 4654
min_duration = 9999999
duration_sum = 0
for job in jobs:
  duration_sum += job.duration
  if (job.duration < min_duration):
    min_duration = job.duration
print("\nAverage job duration:", duration_sum/len(jobs), "seconds")
print("\nMinimum job duration:", min_duration, "seconds")

# This helper method determines how much time each job has remaining
def getTimeRemaining(job) -> float:
  if job.run_time is None:
    return job.duration
  else:
    return job.duration-job.run_time

# A STCF scheduler:
def stcf_scheduler(jobs: List[Job], time_step: float = 1.0):
  # copy the list of jobs!
  not_finished = jobs[:]
  not_finished.sort(key=lambda j:j.arrival_time)
  time = 0
  # Keep track of where we are in the list of not_finished
  i: int = 0
  length: int = len(not_finished)
  # Keep track of alive jobs
  alive: List[Job] = []

  while (i < length or len(alive) != 0):
    # Collect only those jobs that are alive:
    while i < length:
      job = not_finished[i]
      if job.has_arrived(time):
        alive.append(job)
        i += 1
      else:
        break
    
    # If no jobs are alive, increment the time
    if len(alive) == 0:
      time += 100

    # select the job with the shortest time left to go to finish (of those alive)
    # ... ties are broken by order in the list of jobs
    next_job = min(alive, key=getTimeRemaining)

    # run for a time slice or for the amount of time remaining for the job, depending on which is less
    time_to_run = min(time_step, getTimeRemaining(next_job))
    next_job.run(time, time_to_run)

    # update our simulation time
    time += time_to_run

    # delete job if it finished!
    if next_job.finished():
      alive.remove(next_job)

# A lottery scheduler:
def lottery_scheduler(jobs: List[Job], time_step: float = 1.0):
  # Copy the list of jobs!
  not_finished = jobs[:]
  not_finished.sort(key=lambda j:j.arrival_time)
  time = 0
  # Keep track of where we are in the list of not_finished
  i: int = 0
  length: int = len(not_finished)
  # Keep track of alive jobs
  alive: List[Job] = []
  # Keep track of how many tickets are in the lottery
  sum_tickets = 0

  while (i < length or len(alive) != 0):
    # Collect only those jobs that are alive:
    while i < length:
      job = not_finished[i]
      if job.has_arrived(time):
        alive.append(job)
        sum_tickets += job.tickets
        i += 1
      else:
        # Most efficient to sort list of jobs from those with highest number of tickets to lowest
        alive.sort(key=lambda j: j.tickets, reverse=True)
        break

    # Select the job to run probabalistically
    # Decide on a winner and keep a counter variable to keep track of if we've found it yet
    winner = random.uniform(0, sum_tickets)
    counter = 0

    # If no jobs are alive, increment the time
    if len(alive) == 0:
      time += 100
    
    for job in alive:
      counter += job.tickets
      if (counter >= winner):
        # Run this job and break
        # Run for a time slice or for the amount of time remaining for the job, depending on which is less
        time_to_run = min(time_step, getTimeRemaining(job))
        job.run(time, time_to_run)
        # Update our simulation time
        time += time_to_run
        if (job.finished()):
          sum_tickets -= job.tickets
          alive.remove(job)
        break

# A RR scheduler:
def rr_scheduler(jobs: List[Job], time_step: float = 1.0):
  # Copy the list of jobs!
  not_finished = jobs[:]
  not_finished.sort(key=lambda j:j.arrival_time)
  time = 0
  # Keep track of where we are in the list of not_finished
  i: int = 0
  length: int = len(not_finished)
  # Keep track of alive jobs
  alive: List[Job] = []
  
  while (i < length or len(alive) != 0):
    # Collect only those jobs that are alive:
    while i < length:
      job = not_finished[i]
      if job.has_arrived(time):
        alive.append(job)
        i += 1
      else:
        break

    # If no jobs are alive, increment the time
    if len(alive) == 0:
      time += 100

    # circle through the jobs, based on who has not run in the longest time
    # ... break ties using the order of the list
    next_job = min(alive, key=lambda j: j.last_run_time)

    # run for a time slice or for the amount of time remaining for the job, depending on which is less
    time_to_run = min(time_step, getTimeRemaining(next_job))
    next_job.run(time, time_to_run)

    # update our simulation time
    time += time_to_run

    # delete job if it finished!
    if next_job.finished():
      alive.remove(next_job)

# Run the scheduler of your choice
print("\nLottery Scheduler")
lottery_scheduler(jobs, 60)
#print("\nSTCF Scheduler")
#stcf_scheduler(jobs, 60)
#print("\nRR Scheduler")
#rr_scheduler(jobs, 60)

# Evaluations
# Response time, turnaround time, and fairness
def get_response_time(job: Job) -> float:
  return job.start_time-job.arrival_time

def get_turnaround_time(job: Job) -> float:
  return job.end_time-job.arrival_time

# This method gets an idea of the average fairness, though not the exact average
# Goal is to have fairness equal 1
def fairness(jobs: List[Job]) -> float:
  jobs.sort(key=lambda j:j.end_time)
  sum = 0
  # Compare each job to its neighbor
  n: int = len(jobs)-1
  for i in range(n):
    sum += jobs[i].end_time/jobs[i+1].end_time
  average = sum/n
  return average

# Calculate the average turnaround and response times
turnaround_time_sum = 0
response_time_sum = 0
for job in jobs:
  turnaround_time_sum += get_turnaround_time(job)
  response_time_sum += get_response_time(job)

# Write the results to a file and also print them to the terminal
with open('jobs.txt', 'w') as out:
  out.write("STCF Scheduler, time slice 60 sec\n")
  out.write("Average turnaround time:")
  out.write(str(turnaround_time_sum/len(jobs)))
  out.write("\nAverage response time:")
  out.write(str(response_time_sum/len(jobs)))
  out.write("\nFairness:")
  out.write(str(fairness(jobs)))

print("Average turnaround time:", turnaround_time_sum/len(jobs),
      "\nAverage response time:", response_time_sum/len(jobs),
      "\nFairness:", fairness(jobs))