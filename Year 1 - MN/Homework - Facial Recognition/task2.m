% This function creates 4 graphics for a given image,
% representing data about its singular values.

function task2(image, k)
   % Read image into a matrix.
   A = double(imread(image));
   [m n] = size(A);
   [U S V] = svd(A);
   
   y2 = y3 = y4 = [];
   div = sum(diag(S)(1:min(m,n)));
   
   % Compute the values that are about to be plotted
   % based on mathematical formulas:
   for i = 1 : k
     A_k = task1(image, i);
     
     y2 = [y2, sum(diag(S)(1:i))/div];
     y3 = [y3, sum(sum((A - A_k).^2)) / (m*n)];
     y4 = [y4, (m*i + n*i + i) / (m*n)];
   endfor
 
   subplot(2, 2, 1);
   plot([1 : min(m,n)], diag(S));
   title("Graphic 1 - Singular values");
   
   subplot(2, 2, 2);
   plot([1 : k], y2);
   title("Grafic 2 - Information given by the first k values");
   
   subplot(2, 2, 3);
   plot([1 : k], y3);
   title("Grafic 3 - The error of aproximation");
   
   subplot(2, 2, 4);
   plot([1 : k], y4);
   title("Grafic 4 - Data compression rate");
   
   hold on;
end