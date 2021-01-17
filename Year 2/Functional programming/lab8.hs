--          ==> The Lambda Calculus <==

-- 1. Consider the following datatype which encodes λ-expressions: 
data LExpr = Var Char | Lambda Char LExpr | App LExpr LExpr

-- Enroll LExpr in class Show.
instance Show LExpr where
    show (Var c) = [c]
    show (Lambda c expr) = "λ"++(show (Var c))++"."++(show expr)
    show (App expr1 expr2) = "("++(show expr1)++" "++(show expr2)++")"

-- verificare: App (Lambda 'x' (Var 'y')) (Var 'z')

-- 2. Write a function vars which returns a list of variables used in a
-- λ-expression:
vars :: LExpr -> [Char]
vars (Var c) = [c]
vars (Lambda c expr) = c:(vars expr)
vars (App expr1 expr2) = (vars expr1)++(vars expr2)

-- verificare: vars (App (Lambda 'x' (Var 'y')) (Var 'z'))

--  3. Write a function reducible which tests if an expression can be
-- reduced to another. Write tests first! What are the cases when an
-- expression is reducible?
reducible :: LExpr -> Bool
reducible (App (Lambda c expr) f) = True
reducible (App expr1 expr2) = reducible expr2
reducible (Lambda c expr) = reducible expr
reducible _ = False

-- verificare: reducible $ App (Var 'x') (Lambda 'c' (Var 'x'))
-- verificare: reducible $ App (Lambda 'x' (Var 'y')) (Var 'z')

-- 4. Write a function which renames all occurrences of a variable with
-- another, in a λ-expression:
rename :: Char -> Char -> LExpr -> LExpr
rename c1 c2 (Var x) = if x == c1 then (Var c2) else (Var x)
rename c1 c2 (Lambda x expr) = if x == c1
                            then Lambda c2 (rename c1 c2 expr)
                            else Lambda x (rename c1 c2 expr)
rename c1 c2 (App expr1 expr2) = App (rename c1 c2 expr1) (rename c1 c2 expr2)

-- verificare: rename 'x' 'p' (App (Lambda 'x' (Var 'y')) (Var 'x'))

-- 5. Write a function which replaces all occurrences of a variable with a
-- λ-expression, in a λ-expression:
replace :: Char -> LExpr -> LExpr -> LExpr
replace c expr (Var x) = if x == c then expr else (Var x)
replace c expr (App expr1 expr2) = App (replace c expr expr1) (replace c expr expr2)
replace c expr (Lambda x expr') = Lambda x (replace c expr expr')

-- verificare:
-- replace 'x' (Var 'p') (Lambda 'x' (Lambda 'x' (App (Var 'x') (Var 'x'))))

-- 6. Write a function which takes a λ-expression of the form (λx.<body> <arg>) and
-- reduces it in a SINGLE step.
reduce1 :: LExpr -> LExpr
reduce1 (App (Lambda c expr) expr') = replace c expr' expr
reduce1 expr = expr

-- verificare:
-- -> reduce1 (App (Lambda 'x' (Lambda 'y' (App (Var 'x') (Var 'y')))) (Var 'p'))
-- -> reduce1 (App (Lambda 'x' (App (Var 'x') (Var 'x'))) (Var 'y'))
-- adica: (λx.(x x) y)
-- -> reduce1 (App (Lambda 'x' (Lambda 'x' (App (Var 'x') (Var 'x')))) (Var 'y'))
-- adica: (λx.λx.(x x) y)

-- 7. Add two data constructors to the type LExpr so that we can also model
-- functions and applications in uncurry form. Examples: (λx y z.<body>),
-- (f x y z).
data LExpr_u = Var_u Char | Lambda_u [Char] LExpr_u | App_u LExpr_u [LExpr_u]

-- 8. Write a proper display function for these new constructors.
instance Show LExpr_u where
    show (Var_u c) = [c]
    show (Lambda_u list expr) = "λ."++(show_list_char list)++"."++(show expr)
        where
            show_list_char :: [Char] -> String
            show_list_char (c:[]) = [c]
            show_list_char (c:xs) = [c]++" "++(show_list_char xs)
    show (App_u expr list) = "("++(show expr)++" "++(show_list_l list)++")"
        where
            show_list_l :: [LExpr_u] -> String
            show_list_l (expr:[]) = show expr
            show_list_l (expr:xs) = (show expr)++" "++(show_list_l xs)

-- verificare:
-- -> Lambda_u ['x','y','z'] (App_u (Var_u 'f') [(Var_u 'x'),(Var_u 'y')])

-- 9. Write a function luncurry which takes an uncurries λ-expression and
-- transforms it in curry form.
luncurry :: LExpr_u -> LExpr
luncurry (Var_u c) = (Var c)
luncurry (Lambda_u (c:[]) expr) = Lambda c (luncurry expr)
luncurry (Lambda_u (c:xs) expr) = Lambda c (luncurry (Lambda_u xs expr))
luncurry (App_u expr (e:[])) = App (luncurry expr) (luncurry e)
luncurry (App_u expr list) =
    App (luncurry (App_u expr (init list))) (luncurry (last list))

-- verificare:
-- -> luncurry (App_u (Var_u 'f') [(Var_u 'x'),(Var_u 'y')])
-- adica: (f x y) => ((f x) y)
-- -> luncurry (App_u (Var_u 'f') [(App_u (Var_u 'g') [(Var_u 'a')]), (Var_u 'y')])
-- adica: (f (g a) y) => ((f (g a)) y)

-- 10. Write a function lcurry which takes a curried λ-expression and transforms
-- it in uncurry form.
--lcurry :: LExpr -> LExpr_u
--lcurry (Var c) = (Var_u c)
--lcurry (Lambda c expr) = Lambda_u (c:(list_char (lcurry expr)))
--lcurry (App (App e1 e2) e3) = App_u e1 [()]

-- 11. Write the function fv which computes the list of all free variables of
-- a λ-expression.
fv :: LExpr -> [Char]
fv (Var c) = [c]
fv (Lambda c expr) = []
fv (App expr1 expr2) = (fv expr1)++(fv expr2)

-- 12. Write a function bv which computes the list of all bound variables of
-- a λ-expression. 
bv :: LExpr -> [Char]
bv (Var c) = []
bv (Lambda c expr) = (c:(bv expr))
bv (App expr1 expr2) = (bv expr1)++(bv expr2)

-- 14. Implement a function which reduces a reducible λ-expression to an
-- irreducible one. (According to the lecture definition, what happens with
-- λx.(λx.x x) ?
reduce :: LExpr -> LExpr
reduce expr = if reducible expr then (reduce (reduce1 expr)) else expr

-- verificare:
-- -> reduce (App (Lambda 'x' (Var 'x')) (App (Lambda 'y' (Var 'z')) (Var 'w'))