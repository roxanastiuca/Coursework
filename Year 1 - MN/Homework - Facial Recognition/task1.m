% Using single value decomposition (svd), this function
% generates the Ak matrix for a given image converted
% into a matrix.

function A_k = task1(image, k)
   A = double(imread(image));
  [m n] = size(A);
  [U S V] = svd(A);
  
  A_k = U(1:m, 1:k) * S(1:k, 1:k) * V'(1:k, 1:n);

end