-- 1. Enroll the type Extended shown below, in class Eq:
data Extended = Infinity | Value1 Integer

instance Eq Extended where
    Infinity == Infinity = True
    (Value1 x) == (Value1 y) = (x == y)
    _ == _ = False

-- 2. Enroll the type Formula a in class Eq:
data Formula a = Atom a |
                Or (Formula a) (Formula a) |
                And (Formula a) (Formula a) |
                Not (Formula a)
                deriving (Eq)

-- 3. Enroll the type Set a, defined below, in class Num. Implement
-- operation (+) as set reunion and (*) as set intersection. Implement
-- also fromInteger which takes an integer x and returns the set {x}.
data Set a = F (a->Bool)

{--
instance (Num a) => Num (Set a) where
    (F f) + (F g) = F (\x -> ((f x) || (g x)))
    (F f) * (F g) = F (\x -> ((f x) && (g x)))
    fromInteger x = F set where
        set x = True
--}

data ExtendedPlus = PInfinity | MInfinity | Value Integer deriving Show

instance Num ExtendedPlus where
    PInfinity + MInfinity = (Value 0)
    MInfinity + PInfinity = (Value 0)
    PInfinity + PInfinity = PInfinity
    MInfinity + MInfinity = MInfinity
    PInfinity + (Value x) = PInfinity
    (Value x) + PInfinity = PInfinity
    MInfinity + (Value x) = MInfinity
    (Value x) + MInfinity = MInfinity
    (Value x) + (Value y) = Value (x+y)
    PInfinity - MInfinity = PInfinity
    MInfinity - PInfinity = MInfinity
    PInfinity - PInfinity = (Value 0)
    MInfinity - MInfinity = (Value 0)
    PInfinity - (Value x) = PInfinity
    (Value x) - PInfinity = MInfinity
    MInfinity - (Value x) = MInfinity
    (Value x) - MInfinity = PInfinity
    (Value x) - (Value y) = Value (x-y)
    PInfinity * PInfinity = PInfinity
    PInfinity * MInfinity = MInfinity
    MInfinity * PInfinity = MInfinity
    MInfinity * MInfinity = PInfinity
    PInfinity * (Value x) = if x > 0 then PInfinity else MInfinity
    (Value x) * PInfinity = if x > 0 then PInfinity else MInfinity
    MInfinity * (Value x) = if x > 0 then MInfinity else PInfinity
    (Value x) * MInfinity = if x > 0 then MInfinity else PInfinity
    (Value x) * (Value y) = Value (x*y)
    abs PInfinity = PInfinity
    abs MInfinity = PInfinity
    abs (Value x) = Value (abs x)
    signum PInfinity = Value 1
    signum MInfinity = Value (-1)
    signum (Value x) = Value (signum x)
    fromInteger x = Value x

-- 4. In the last lab, we have worked with the following datatypes and functions:
-- FACUT IN lab6.hs