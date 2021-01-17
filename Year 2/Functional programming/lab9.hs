--				==> LAZY EVALUATION <==

nats = 0:(map (+1) nats)
evens = zipWith (+) nats nats
fibo = 0:1:(zipWith (+) fibo (tail fibo))

-- 1. Build the sequence 1, 2, ..., n!, ...
fact = 1:(zipWith (*) fact [1..])

-- 2. Define the sequence 1, 1/2, ..., 1/k!, ...
subfact = map (\x -> 1/x) fact

-- 3. Write a function which takes a sequence (an) and computes the
-- sequence (sn) where sk = sum ak.
sum_seq (h:t) = h:(map (+h) (sum_seq t))

-- 4. Write the stream of approximations of e.
aprox_e = map (+1) (sum_seq subfact)

-- 5. Write a function which takes and f, a value a0 and returns
-- ak from the sequence which satisfies the condition |ak - ak+1| <= d.
take_tol e (x:y:xs) = if (abs(x-y) < e) then x else take_tol e (y:xs)

-- 6. Write a function which takes an f , a value a0 and computes
-- the sequence a0,f(a0),f(f(a0)),…
multicall f a = a : (map (\x -> f x) (multicall f a))

-- 7. The sequence (an)n≥0 defined as ak+1=(ak+nak)/2 , will converge
-- to √n as k approaches infinity. Use it to write a function which
-- approximates √n within 3 decimals.
sqrt_val n = multicall (\x -> (x + n/x)/2) 10
get_aprox n = take_tol 0.0001 (sqrt_val n)