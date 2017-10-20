%interpolate_banda_ant_model.m
% Used to convert from latitude/longitude/depth/vs after outputs from
% herrmann's inversion to evently sampled latitude/longitude/depth/vs
% Big complication here is that herrmann's code outputs the depth in terms
% of layer thickness, which I've simplified to vs at the middle of the
% layer. This code uses the scatteredInterpolant function to re-evaluate
% the velocity on the desired spatial basis. Some slight smoothing is added
% to make the visuals cleaner


% Sets the file name
inputfile='/home/rmartinshort/Documents/Berkeley/Ambient_Noise/AmbientNoise/Scripts/Depth_Invert/Station_grid/Alaska_stations_surf/alaskastations.3d.mod';
outputfile='/home/rmartinshort/Documents/Berkeley/Ambient_Noise/AmbientNoise/Scripts/Depth_Invert/Station_grid/Alaska_stations_surf/alaskastations.3d.mod.smooth';

% Open and read the file
fid = fopen(inputfile,'r');
C=textscan(fid,'%f %f %f %f');
fclose(fid);

% Create a function we can use to look up the nearest velocity at a point
disp('Creating the velocity function')
F=scatteredInterpolant(C{1}, C{2}, C{3}, C{4});
F.Method = 'nearest';
F.ExtrapolationMethod = 'nearest';

% Set the output geometry for the model
xvector = 201.2:0.2:224.2;
yvector = 58.2:0.2:68.8;
zvector = 1:1:200;


% Create a matlab mesh
[xmesh, ymesh, zmesh] = meshgrid(xvector, yvector, zvector);

% evaluate the velocity on the mesh
disp('Evaluating velocity on mesh')
vel = F(xmesh, ymesh, zmesh);

% Smooth
nsmoothx = 3; 
nsmoothy = 3;
nsmoothz = 3;
disp('Evaluating smoothing matrix')
vel = smooth_nxmxk_matrix(vel,nsmoothx, nsmoothy, nsmoothz);

% write
disp('Writing to file')
fid = fopen(outputfile,'w');
for idx=1:length(xvector)
    for idy=1:length(yvector)
        for idz=1:length(zvector)
            fprintf(fid,'%3.1f %3.1f %3.1f %3.5f\n',xmesh(idy, idx, idz), ymesh(idy, idx, idz), zmesh(idy, idx, idz), vel(idy, idx, idz));
        end
    end
end
fclose(fid);
fprintf('Done with file %s.\n',outputfile);


