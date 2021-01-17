%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%			Introduction to Prolog programming - LAB 1 PP			%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

% Consider the following representation of natural numbers
% (from our last lecture):

nat(zero).
nat(X) :- X = succ(Y), nat(Y).

% 1. Implement the predicate add/3 which adds two natural numbers.

add(zero, Y, Y).
add(succ(X), Y, succ(Res)) :- add(X, Y, Res).

% verificare:
% add(succ(zero), succ(succ(zero)), Y). -> Y = succ(succ(succ(zero))).


% 2. Implement the predicate minus/3 which substracts one natural number
% from another. Substraction only needs to work as long as the result is
% a natural number.

minus(X, zero, X).
minus(X, succ(Y), Res) :- minus(X, Y, succ(Res)).

% verificare:
% minus(succ(succ(zero)), succ(zero), Y). -> Y = succ(zero).

% Consider the following conversion predicates between naturals and
% Prolog integers.

toNat(0, zero).
toNat(X, succ(Res)) :- X>0, Xp is X-1, toNat(Xp, Res).

fromNat(zero, 0).
fromNat(succ(X), R) :- fromNat(X, Rp), R is Rp+1.

% 3. Implement min/3 which computes the minimum of two natural numbers.

min(zero, Y, zero) :- nat(Y).
min(X, zero, zero) :- nat(X).
min(succ(X), succ(Y), succ(Res)) :- min(X, Y, Res).

% verificare:
% toNat(7,X), toNat(4,Y), min(X,Y,R), fromNat(R,O). -> O = 4.

%  4. Implement max/3 which computes the maximum of two natural numbers.

max(X, zero, X).
max(zero, Y, Y).
max(succ(X), succ(Y), succ(Res)) :- max(X, Y, Res).

% verificare:
% toNat(7,X), toNat(4,Y), max(X,Y,R), fromNat(R,O).

% 5. Implement gt/2 /which is satisfied if the first natural number is
% strictly greater than the second.

% gt(X, Y) :- max(X, Y, X), X \= Y.

gt(succ(X), zero) :- nat(X).
gt(succ(X), succ(Y)) :- gt(X, Y).

% verificare:
% toNat(5,X), toNat(4,Y), gt(X, Y).

% 6. Implement leq/2 which is satisfied if the first natural number is
% less than or equal to the second.

leq(X, X) :- nat(X).
leq(X, Y) :- gt(Y, X).

% 7. Implement div/3 which implements the div operator. 

% idee: X/Y=Res => (X-Y)/Y=R, Res = R+1

div(X, zero, zero) :- nat(X).
div(X, Y, zero) :- gt(Y, X).
div(X, Y, R) :- div(X1, Y, R1), add(X1, Y, X), add(R1, succ(zero), R).

% verificare:
% toNat(7,X), toNat(2,Y), div(X,Y,Z), fromNat(Z,O).

% 8. Implement mod/3.

mod(X, zero, zero) :- nat(X).
mod(X, Y, X) :- gt(Y, X).
mod(X, Y, R) :- mod(X1, Y, R), add(X1, Y, X).

% verificare:
% toNat(11,X), toNat(3,Y), mod(X,Y,Z), fromNat(Z,O).

% 9. Implement gcd/3 which computes the greatest common divisor of two
% natural numbers.

% gcd(X, zero, X).
% gcd(X, Y, R) :- gcd(Y, M, R), mod(X, Y, M).

gcd(X, X, X).
gcd(X, Y, R) :- gt(X, Y), add(X1, Y, X), gcd(X1, Y, R).
gcd(X, Y, R) :- gt(Y, X), add(X, Y1, Y), gcd(X, Y1, R).

% verificare:
% toNat(24,X), toNat(16,Y), gcd(X,Y,Z), fromNat(Z, O).

% Consider the following representation of lists, expressed by the
% predicate isList shown below.

isList(void).
isList(cons(_, T)) :- isList(T).

% 10. Implement head/2 and tail/2.

head(cons(H, _), H).
tail(cons(_, T), T).

% verificare:
% head(cons(1,cons(2,cons(3,void))), X). -> X = 1.
% tail(cons(1,cons(2,cons(3,void))), X). -> X = cons(2, cons(3, void)).

% 11. Implement size/2 which determines the length of a list as a Prolog
% integer.

size(void, 0).
size(cons(_, T), R) :- size(T, R1), R is R1+1.

% verificare:
% size(cons(1,cons(2,cons(3,void))), X). -> X = 3.

% 12. Implement concat/3 which concatenates two lists.

concat(void, L2, L2).
concat(cons(X, L1), L2, R) :- concat(L1, L2, R1), R = cons(X, R1).

% verificare:
% concat(cons(1,cons(2,void)), cons(3, cons(4, cons(5, void))), X).

% 13. Implement reverse/2 /which reverses a list. Use accumulators instead
% of concatenation and an auxiliary predicate.

reverse(void, void).
reverse(cons(H, void), cons(H, void)).
reverse(cons(H, T), R) :- reverse(T, Tr), concat(Tr, cons(H, void), R).

% verificare:
% reverse(cons(1,cons(2,cons(3,void))), X).

% Consider the following conversion functions from the above list
% representation to Prolog lists.

fromList(void,[]).
fromList(cons(H,T),[H|R]) :- fromList(T,R).

% 14. Write the predicate toList/2 which convers a Prolog list to our
% first representation.

toList([], void).
toList([H|T], cons(H, R)) :- toList(T, R).

% verificare:
% toList([1,2,3],X).

% 15. Implement kelem/3 which determines the k-th element of a Prolog list.

kelem(0, [H|_], H). % indexat de la 0.
kelem(K, [_|T], R) :- kelem(K1, T, R), K is K1+1.

% verificare:
% kelem(4, [1,2,3,4,5,6,7,8], X). -> X = 5.

% 16. Implement rem/2 which removes consecutive duplicates from a Prolog
% list.

rem([], []).
rem([H|[]], [H|[]]).
rem([H|[H|T]], R) :- rem([H|T], R).
rem([H1|[H2|T]], [H1|R]) :- H1 \= H2, rem([H2|T], R).

% verificare:
% rem([1,1,1,2,2,3,4,4],R). -> R = [1,2,3,4].

% 17. Implement flatten/2 which flattens nested Prolog lists. Do not use
% auxiliary predicates.

flatten([], []).
flatten([H|T], R) :- flatten(H, H1), flatten(T, T1), append(H1, T1, R).
flatten(E, [E]).

% verificare:
% flatten([1,2,[3,4,[5]], [[6],[]], [7]], R). -> X = [1,2,3,4,5,6,7].

% 18. Implement pack/2/ which groups consecutive duplicates into sublists.

pack([H|[]], [[H]]).
pack([H|T], [[H | [H|T1]] | T2]) :- pack(T, [[H|T1]|T2]).
pack([H|T], [ [H] | [[H1|T1]|T2] ]) :- pack(T, [[H1|T1]|T2]), H \= H1.

% verificare:
% pack([1,1,1,2,3,3,1,1,4,5,5,5],R).

% 19. Implement slice/4 which returns the elements of a list between two
% positions.

slice([H|_], 0, 0, [H]).
slice([H|T], 0, P2, [H|R]) :- P is P2-1, slice(T, 0, P, R).
slice([_|T], P1, P2, R) :- P11 is P1-1, P22 is P2-1, slice(T, P11, P22, R).

% verificare:
% slice([1,2,3,4,5,6,7,8,9,10], 3, 7, R). -> R = [4,5,6,7,8].

% 20. Implement rotate/3 which rotates a list by a given number of
% positions.

rotate(L, 0, L).
rotate([H|T], N, R) :- N1 is N-1, append(T, [H], R1), rotate(R1, N1, R).

% verificare:
% rotate([1,2,3,4,5,6,7,8],3,R).