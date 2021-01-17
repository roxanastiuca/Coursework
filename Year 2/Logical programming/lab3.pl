%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%				Programming with cut and not - LAB 3 PP				%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

% A tic-tac-toe table is encoded as a list [ [X1,Y1,Z1], [X2,Y2,Z2],
% [X3,Y3,Z3] ] where a variable is bound to atom x or z (zero) if the
% position has been played or it is uninstantiated otherwise.

% 1. Write the predicate diags(Tbl,R) which computes the list R of
% diagonals in table Tbl.
diags([[X1,_,Z1],[_,Y2,_],[X3,_,Z3]], [[X1,Y2,Z3], [Z1,Y2,X3]]).

% 2. Write the predicate cols(Tbl,R) which computes the list of columns
% in table Tbl. Hint: use maplist/3.
cols([[X1,Y1,Z1],[X2,Y2,Z2],[X3,Y3,Z3]],
	[[X1,X2,X3],[Y1,Y2,Y3],[Z1,Z2,Z3]]).

% 3. Write the predicate winner(X,Tbl) which verifies if Tbl is winning
% for player X.
winner(X, Tbl) :-
	diags(Tbl, Diags), append(Tbl, Diags, Laux),
	cols(Tbl, Cols), append(Laux, Cols, L),
	check(X, L).

check(X, [H|_]) :- is_only_member(X, H), !.
check(X, [_|T]) :- check(X, T).

is_only_member(_, []).
is_only_member(X, [H|T]) :- not(var(H)), X = H, is_only_member(X, T).

% 4. Write the predicate succ/2 where -? R = SomePos, succ(X,SomePos)
% binds variable R to all possible moves which player X can play in
% SomePos.
succ(X, R) :- member(L, R), member(Q, L), var(Q), Q = X.

% 5. Write a predicate csp(X,G,Dom,R) where X is an uninstantiated
% variable, G is a goal which contains the uninstantiated variable X,
% Dom is a list of possible values for X. R will be bound to the set of
% values for X from Dom which satisfy G.
csp(_, _, [], []).
csp(X, G, [H|T], Tr) :- not((X=H, G)), csp(X, G, T, Tr), !.
csp(X, G, [H|T], [H|Tr]) :- csp(X, G, T, Tr).

% 6. Write the predicate ncsp(Vars,G,Doms,Tuples).


% verificare:
% winner(x, [[x,x,z],[z,x,_],[_,z,x]]). -> true
% winner(x, [[x,x,z],[z,_,_],[_,z,x]]). -> false
% R = [[_,_,_],[z,_,x],[x,_,z]], succ(x,R).
% csp(X, (mod(X,2) =:= 0), [1,2,3,4,5,6,7], R).