
-- 1. Define the type list with elements of type Integer:
data IntList = Empty | Cons Integer IntList deriving Show

-- 2. Define a function which computes the sum of elements of such a list:
isum :: IntList -> Integer
isum Empty = 0
isum (Cons x l) = x + (isum l)

-- 3. Define type polymorfic type List a encoding lists with elements of type a: 
data List a = PEmpty | PCons a (List a) deriving Show

-- 4. Define a function which converts IntList lists to List Integer lists: 
to_poly_list :: IntList -> List Integer
to_poly_list Empty = PEmpty
to_poly_list (Cons x l) = PCons x (to_poly_list l)
-- verificare: to_poly_list (Cons 1 (Cons 2 Empty))

-- 5. Define a function which displays lists. What type will this function have? 
show_list :: Show a => List a -> String
show_list PEmpty = "[]"
show_list (PCons x l) = (show x)++":"++(show_list l)
-- verificare: show_list $ PCons 12 $ PCons 5 PEmpty

-- Add the tree datatype from the lecture:
data Tree a = Void | Node (Tree a) a (Tree a) deriving Show

-- 6. Implement the function flatten:
flatten :: Tree a -> List a
flatten Void = PEmpty
flatten (Node l k r) = (flatten l) `app` (PCons k (flatten r))
-- verificare: flatten (Node (Node Void 3 (Node Void 4 Void)) 1 (Node Void 2 Void))

--  7. Define list concatenation over type List a: 
app :: (List a) -> (List a) -> (List a)
app PEmpty l2 = l2
app l1 PEmpty = l1
app (PCons x l1) l2 = PCons x (app l1 l2)
-- verificare: app (PCons 3 (PCons 4 PEmpty)) (PCons 1 (PCons 2 PEmpty))

-- 8. Define the function tmap which is the Tree a correspondent
-- to map::(a→b) → [a] → [b].
tmap :: (a -> b) -> (Tree a) -> (Tree b)
tmap _ Void = Void
tmap f (Node l k r) = Node (tmap f l) (f k) (tmap f r)
-- verificare: tmap (*10) (Node Void 2 (Node Void 3 (Node Void 4 Void)))

-- 9. Define the function tzipWith:
tzipWith :: (a -> b -> c) -> (Tree a) -> (Tree b) -> (Tree c)
tzipWith f Void Void = Void
tzipWith f (Node l1 k1 r1) (Node l2 k2 r2) = Node (tzipWith f l1 l2) (f k1 k2) (tzipWith f r1 r2)
-- verificare: tzipWith (+) (Node Void 1 Void) (Node Void 2 Void)
-- verificare: tzipWith (:) (Node Void 1 Void) (Node Void [2,3] Void)

-- 10. Define the function tfoldr: 
tfoldr :: (a -> b -> b) -> b -> Tree a -> b
tfoldr f acc Void = acc
tfoldr f acc (Node l k r) = tfoldr f (f k (tfoldr f acc r)) l
-- verificare: tfoldr (+) 0 (Node (Node Void 5 Void) 1 (Node Void 3 Void))

-- 11. Implement the flattening function using tfoldr: 
tflatten :: Tree a -> List a
tflatten t = tfoldr (PCons) PEmpty t

-- 12. Consider the following definition of natural numbers extended with the value Infinity:
data Extended = Infinity | Value Integer

-- Define a function which computes the sum of two Extended values: 
extSum :: Extended -> Extended -> Extended
extSum Infinity _ = Infinity
extSum _ Infinity = Infinity
extSum (Value x) (Value x') = Value (x + x')

-- 13. Define a function which computes the equality of two Extended values: 
equal :: Extended -> Extended -> Bool
equal Infinity Infinity = True
equal (Value x) (Value y) = x == y

-- The polymorphic datatype: 
-- data Maybe a = Nothing | Just a

-- 14. Implement the function lhead which behaves like head but returns
-- Nothing if the argument of the function is the empty list:
lhead :: List a -> Maybe a
lhead PEmpty = Nothing
lhead (PCons x l) = Just x

-- 15. Implement the function ltail
ltail :: List a -> Maybe a
ltail PEmpty = Nothing
ltail (PCons x PEmpty) = Just x
ltail (PCons x l) = ltail l
-- verificare: ltail (PCons 5 (PCons 7 (PCons 6 PEmpty)))