%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%					Generative predicates - LAB 2 PP				%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

% 1. Write the predicate sublist/2 which constructs each sublist of a
% given list. Hint: use append.
% 2. Rewrite the predicate such that the empty list is reported only once.
sublist(_, []).
sublist(L, R) :- append(S, _, L), append(_, R, S), R \= [].

% 3. Write the predicate natlist/1 which generates each finite list of
% natural number in ascending order.
natlist_gen(_, []).
natlist_gen(N, [N|R]) :- Next is N+1, natlist_gen(Next, R).

natlist(R) :- natlist_gen(0, R).

% 4. Write the predicate oddOnly/2 which removes all even integers from a
% list.
oddOnly([], []).
oddOnly([H|T], [H|Tr]) :- M is mod(H,2), M is 0, oddOnly(T, Tr), !.
oddOnly([_|T], Tr) :- oddOnly(T, Tr).

% 5. Write the predicate oddList/1 which generates all finite lists of
% odd numbers, in ascending order. 
oddList(R) :- natlist(L), oddOnly(L, R).

% 6. Write a predicate eqelem/1 which generates all lists of uninstantiated
% variables which must be equal. Example: 
eqelem([]).
eqelem([_]).
eqelem([X|[X|T]]) :- eqelem([X|T]).

% 7. Use the previous predicate to write repeat/3 which returns a list
% containing the same value X repeated K times. 
repeat(X, K, L) :- L = [X|_], length(L, K), eqelem(L).

% 8. Write a predicate pal/1 which generates all lists of palindromes.
% Hint: use only append.
pal([]).
pal([_]).
pal(L) :- append([X], Mij, S), append(S, [X], L), pal(Mij).

% 9. Write a predicate ksubset/3 where ksubset(C,K,V) generates all sets
% (represented as lists) C with K elements from the list V. Hint: build the
% recursion scheme after K. 
% ksubset([X], 1, V) :- member(X, V).
% ksubset([X|T], K, V) :-
%	member(X, V), delete(V, X, Vp), Kp is K-1, ksubset(T, Kp, Vp).

ksubset([], 0, _).
ksubset([H|C], K, [H|T]) :- Kp is K-1, ksubset(C, Kp, T).
ksubset(C, K, [_|T]) :- ksubset(C, K, T).

% 10. Write a predicate subset where subset(C,V) generates all subsets of V.
% Hint: build the recursion scheme after each element in V.
%subset([X], V) :- member(X, V).
%subset([X|T], V) :-
%	member(X, V), delete(V, X, Vp), subset(T, Vp).

subset([], []).
subset([H|C], [H|T]) :- subset(C, T).
subset(C, [_|T]) :- subset(C, T).