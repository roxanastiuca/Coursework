% This function determines if a given image is that of a face
% or not, based on information extracted from a dataset of face
% images.

function [min_dist output_img_index] = face_recognition(image_path, m, A, eigenfaces, pr_img)
  % Read image:
  matrix = double(rgb2gray(imread(image_path)));
  T = matrix'(:);
  med = mean(T);
  Vect = T - m;
  
  % The image's projection:
  pr_test_img = eigenfaces' * Vect;
  
  min_dist = inf;
  output_img_index = 0;
  [nl nc] = size(pr_img);

  % Find the minimum distance of that image to a face image
  % from the dataset and save its index:
  for i = 1:nc
    if norm(pr_test_img - pr_img(:,i)) < min_dist
      min_dist = norm(pr_test_img - pr_img(:,i));
      output_img_index = i;
    endif
  endfor

end