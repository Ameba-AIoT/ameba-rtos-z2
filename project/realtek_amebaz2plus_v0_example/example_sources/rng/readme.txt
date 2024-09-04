Example Description

This example describes how to use rng function

zii rng uses gtimer0 & secure crypto engine, so when user uses rng function in secure state, makesure has already initialized secure crypto.

If user uses rng function in non-secure state, users still uses secure crypto engine, but it does not need to initialize, because 

rng nsc function has wrapped the secure crypto init function.

No matter user uses rng function in secure or non-secure state, it's necessary using device lock to protect the access of secure engine.

to run in non secure
make and enter vrf_rng in console
to run in secure:
make tz and enter vrf_rng in console

the example will print out random number in hexadecimal