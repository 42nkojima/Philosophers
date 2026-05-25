*This project has been created as part of the 42 curriculum by nkojima.*

## Description

**Philosophers** is the mandatory threading exercise from the 42 curriculum.
The `philo` program simulates the dining philosophers problem: each philosopher
runs in its own thread, shares forks with neighbors, and must eat, sleep, and
think without data races, deadlocks, or incorrect death detection.

The goal is to practice shared-state protection with `pthread_mutex_t`, correct
lock ordering, and a monitor thread that detects death within 10 ms and stops
the simulation when a philosopher dies or when every philosopher has eaten the
required number of meals (optional fifth argument).

## Instructions

### Build

```sh
make -C philo
```

### Run

```sh
./philo/philo number_of_philosophers time_to_die time_to_eat time_to_sleep \
  [number_of_times_each_philosopher_must_eat]
```

Examples:

```sh
./philo/philo 5 800 200 200
./philo/philo 5 800 200 200 7
./philo/philo 1 800 200 200
```

### Tests

```sh
make -C philo test
make -C philo norm
```

## Resources

- [Dining philosophers problem (Wikipedia)](https://en.wikipedia.org/wiki/Dining_philosophers_problem)
- [POSIX threads (man7)](https://man7.org/linux/man-pages/man7/pthreads.7.html)
- [pthread_mutex_lock(3)](https://man7.org/linux/man-pages/man3/pthread_mutex_lock.3.html)
- [gettimeofday(2)](https://man7.org/linux/man-pages/man2/gettimeofday.2.html)
- Project subject: `en.subject.pdf` in this repository
- Design notes (Japanese): `docs/design.md`

### AI usage

AI tools (Cursor / Claude) were used to:

- Draft and refine `docs/design.md` (mutex roles, lock order, test matrix)
- Review concurrency and subject-compliance issues on feature branches
- Suggest fixes for optional meal-count termination and fork-acquisition timing

All simulation logic, mutex layout, and final code were written and verified
by the author (build, unit tests, and manual scenario runs).
