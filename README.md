*This project has been created as part of the 42 curriculum by nkojima.*

## Description

**Philosophers** is the mandatory threading exercise from the 42 curriculum.
The `philo` program simulates the dining philosophers problem: each philosopher
runs in its own thread, shares forks with neighbors, and must eat, sleep, and
think without data races, deadlocks, or incorrect death detection.

The goal is to practice shared-state protection with `pthread_mutex_t`, correct
lock ordering (resource hierarchy on forks), and a monitor thread that detects
death within 10 ms and stops the simulation when a philosopher dies or when
every philosopher has eaten the required number of meals (optional fifth
argument).

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

- [Philo project notes (Zenn)](https://zenn.dev/tokochiz/scraps/7d7a0323f85201) — threads, mutexes, deadlock, stagger, and common eval scenarios
- [(outdated) What was philosopher? (Qiita)](https://qiita.com/42yliu/items/86d16cdbc584c250ca6e) — Coffman conditions, deadlock prevention vs avoidance, starvation (legacy subject)
- [Dining philosophers problem (Wikipedia, JA)](https://ja.wikipedia.org/wiki/%E9%A3%9F%E4%BA%8B%E3%81%99%E3%82%8B%E5%93%B2%E5%AD%A6%E8%80%85%E3%81%AE%E5%95%8F%E9%A1%8C) — classic problem statement and solutions (hierarchy, waiter, Chandy/Misra)
- [Slides: understanding Philosophers (Google)](https://docs.google.com/presentation/d/12-lAykLu-RVACE1gI2aP-uEYZoOaeeFVYGh8W4ttTNw/edit?slide=id.gd4524b1be8_0_253#slide=id.gd4524b1be8_0_253) — 42-oriented overview
- [Tsukuba CS lecture (2020-05-08)](https://www.cs.tsukuba.ac.jp/~yas/cs/csys-2020/2020-05-08/index.html) — concurrent systems course notes
- [Concurrent vs parallel processing (Qiita)](https://qiita.com/Kohei909Otsuka/items/26be74de803d195b37bd) — process vs thread, concurrent vs parallel terminology
- [Dining philosophers deadlock in C (Qiita)](https://qiita.com/KenjiOtsuka/items/2355963f826ba2a9edd8) — deadlock demo and ordered fork acquisition
- [Threads/processes vs concurrent/parallel (Zenn)](https://zenn.dev/chro96/articles/abcda94d41697b) — why these concepts should not be conflated
- Project subject: `en.subject.pdf` in this repository
- Internal design notes: `docs/design.md` (Japanese)

### How AI was used

AI tools (Cursor / Claude) were used for:

- Drafting and refining `docs/design.md` (mutex roles, global lock order, test matrix)
- Multi-agent review of concurrency and subject compliance on feature branches
- Suggesting fixes for optional meal-count termination, fork acquisition, and README wording

All simulation logic, mutex layout, and final code were written and verified by the
author (build, unit tests, and manual scenario runs).
