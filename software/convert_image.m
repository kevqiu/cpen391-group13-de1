% to run script:
% initialize img as the filename (eg. pic.jpg will be just 'pic'
% remember to remove commas at the end of each array
img_file = strcat(img, '.jpg');
Img = imread(img_file);
filename = fopen(strcat(img, '.txt'), 'wt');

fprintf(filename, 'const int ');
fprintf(filename, img);
fprintf(filename, '_R[] = {');
fprintf(filename, '%i,', Img(:,:,1).');
fprintf(filename, '};\n\n');

fprintf(filename, 'const int ');
fprintf(filename, img);
fprintf(filename, '_G[] = {');
fprintf(filename, '%i,', Img(:,:,2).');
fprintf(filename, '};\n\n');

fprintf(filename, 'const int ');
fprintf(filename, img);
fprintf(filename, '_B[] = {');
fprintf(filename, '%i,', Img(:,:,3).');
fprintf(filename, '};\n\n');

fclose(filename);