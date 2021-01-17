{-# LANGUAGE FlexibleContexts #-}
{-# LANGUAGE MultiParamTypeClasses #-}
{-# LANGUAGE TypeSynonymInstances #-}
{-# LANGUAGE FlexibleInstances #-}

-- 1. A dictionary is a collection of key-value pairs, which we can represent
-- in Haskell as a list of pairs (String,Integer) where String is the key type
-- and Integer is the value type:
type Dict = [(String, Integer)]

-- Implement the function valueOf which takes a key and a dictionary, and
-- returns the associated value. It is guaranteed that the value exists.
valueOf :: String -> Dict -> Integer
valueOf str = foldr (\(x, y) acc -> if x == str then y else acc) 0
-- verificare: valueOf "ana" [("andrei",4),("casa",6),("ana", 1)]

-- 2. Implement the function ins which takes a key s, a value i, a dictionary
-- d and updates the value of s in the dictionary, if the value exists, or adds
-- the key-value pair, otherwise. For instance ins “x” 1 [(“x”,0)] = [(“x”,1)]
-- and ins “x” 1 [(“y”,0)] = [(“x”,1),(“y”,0)].
ins :: String -> Integer -> Dict -> Dict
ins s i [] = [(s, i)]
ins s i ((x, y):ys) = if x == s then ((x, i):ys) else (x, y):(ins s i ys)
-- verificare: ins "ana" 6 [("andrei",4),("casa",6),("ana", 1)]
-- verificare: ins "mere" 6 [("andrei",4),("casa",6),("ana", 1)]

-- 3. Consider the type PExpr of program expressions defined in the lecture.
-- Implement a function for showing PExpr values:
data PExpr = Val Integer |
            Var String |
            PExpr :+: PExpr

show_pexpr :: PExpr -> String
show_pexpr (Val int) = show int
show_pexpr (Var str) = str
show_pexpr (e1 :+: e2) = (show_pexpr e1)++"+"++(show_pexpr e2)
-- verificare: show ((Val 5) :+: ((Var "x") :+: (Var "y")))

instance Show PExpr where
    show = show_pexpr

-- 4. Implement a function which takes a dictionary of program variables, a
-- program expression PExpr, and evaluates it. For instance: eval_pexpr
-- [(“x”,2),(“y”,1)] ( (Var “x”) :+: (Var “y”) ) returns 3.
eval_pexpr :: Dict -> PExpr -> Integer
eval_pexpr dict (Val int) = int
eval_pexpr dict (Var str) = valueOf str dict
eval_pexpr dict (e1 :+: e2) = (eval_pexpr dict e1)+(eval_pexpr dict e2)
-- verificare: eval_pexpr [("x", 2),("y", 1)] ((Var "x"):+:(Var "y"))

-- 5. Consider the type BExpr of boolean expressions defined in the lecture.
-- Implement a function for displaying values of BExpr:
data BExpr = PExpr :==: PExpr |
            PExpr :<: PExpr |
            Not BExpr |
            BExpr :&&: BExpr

show_bexpr :: BExpr -> String
show_bexpr (e1 :==: e2) = (show_pexpr e1)++"=="++(show_pexpr e2)
show_bexpr (e1 :<: e2) = (show_pexpr e1)++"<"++(show_pexpr e2)
show_bexpr (Not e) = "~"++(show_bexpr e)
show_bexpr (e1 :&&: e2) = (show_bexpr e1)++"&&"++(show_bexpr e2)
--verificare: show ((Var "x"):==:((Var "y"):+:(Val 1)))

instance Show BExpr where
    show = show_bexpr 

-- 6. Write a function which, given a dictionary, evaluates boolean conditions
-- BExpr:
eval_bexpr :: Dict -> BExpr -> Bool
eval_bexpr dict (e1 :==: e2) = (eval_pexpr dict e1)==(eval_pexpr dict e2)
eval_bexpr dict (e1 :<: e2) = (eval_pexpr dict e1)<(eval_pexpr dict e2)
eval_bexpr dict (Not e) = not (eval_bexpr dict e)
eval_bexpr dict (e1 :&&: e2) = (eval_bexpr dict e1)&&(eval_bexpr dict e2)

-- Add the following code (from the lecture) to your program:
type Var = String

data Prog = PlusPlus Var |        -- x++;
            Var :=: PExpr |     -- x = <expr>;
            DeclareInt Var |      -- int x;
            Begin Prog Prog |     -- <p> <p'>
            While BExpr Prog |     -- while (<expr>) { <p> }
            If BExpr Prog Prog      -- if (<expr>) { <p> } else { <p'> }   
 
show_p :: Prog -> String
show_p (PlusPlus v) = v++"++;\n"
show_p (x :=: e) = x++"="++(show_pexpr e)++";\n"
show_p (DeclareInt x) = "int "++x++";\n"
show_p (Begin p p') = (show_p p)++(show_p p')
show_p (While e p) = "while ("++(show_bexpr e)++") {\n"++(show_p p)++"}\n"
show_p (If e p p') = "if ("++(show_bexpr e)++") {\n"++(show_p p)++"}\n else {\n"++(show_p p')++"}\n"
 
instance Show Prog where
    show = show_p

-- 7. Define the program:
{-
int x;
x++;
while (x < 100) {
    x = x + 1;
}
-}

prog7 = Begin (DeclareInt "x") $ 
        Begin (PlusPlus "x") $
        While ((Var "x") :<: (Val 100)) (
                ("x" :=: ((Var "x") :+: (Val 1)))
            )
-- verificare: prog7

-- 8. Define a function eval which takes a dictionary and a program, and evaluates
-- the program under the given dictionary. The function will return an updated
-- dictionary. For instance:
eval_prog :: Dict -> Prog -> Dict
eval_prog dict (PlusPlus str) = ins str ((valueOf str dict)+1) dict
eval_prog dict (str :=: e) = ins str (eval_pexpr dict e) dict
eval_prog dict (DeclareInt str) = dict -- ins str 0 dict
eval_prog dict (Begin p1 p2) = eval_prog (eval_prog dict p1) p2
eval_prog dict (While b p) = eval_prog dict p
eval_prog dict (If b p1 p2) = eval_prog (eval_prog dict p1) p2

-- Add the following error-handling type to your program: 
data Result a = Error String |
            Value a

-- 9. Implement the function check which takes a list of defined variables, a
-- program p, and returns an updated list of variable definitions, if p does not
-- contain undeclared variables, and an error message, otherwise.
check_pexpr :: Result [Var] -> PExpr -> Result [Var]
check_pexpr (Value l) (Val int) = Value l
check_pexpr (Value l) (Var str) = if elem str l 
                                then (Value l)
                                else (Error (str++" not defined"))
check_pexpr (Value l) (e1 :+: e2) = check_pexpr (check_pexpr (Value l) e1) e2
check_pexpr (Error str) _ = Error str

check :: Result [Var] -> Prog -> Result [Var]
check (Value l) (PlusPlus str) = if elem str l 
                                then (Value l)
                                else (Error (str++" not defined"))
check (Value l) (str :=: e) = if elem str l
                            then check_pexpr (Value l) e
                            else (Error (str++" not defined"))
check (Value l) (DeclareInt str) = Value (str:l)
check (Value l) (Begin p1 p2) = check (check (Value l) p1) p2
check (Value l) (While bexp p) = check (Value l) p
check (Value l) (If bexp p1 p2) = check (check (Value l) p1) p2
check (Error str) _ = (Error str)

-- Lab 7:
class Eval a b where
    eval :: Dict -> a -> b

instance Eval PExpr Integer where
    eval dict pexpr = eval_pexpr dict pexpr

instance Eval BExpr Bool where
    eval dict bexpr = eval_bexpr dict bexpr

instance Eval Prog Dict where
    eval dict prog = eval_prog dict prog