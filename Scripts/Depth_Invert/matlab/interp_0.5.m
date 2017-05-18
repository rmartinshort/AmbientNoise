
%interpolate 3d model
%copy of Rob's script to interpolate a depth-vs ambient noise model

inputfile='/home/rmartinshort/Documents/Berkeley/Ambient_Noise/Depth_invert/ForHermannInvert_0.5_const/alaska.3d.mod';
outputfile='/home/rmartinshort/Documents/Berkeley/Ambient_Noise/Depth_invert/ForHermannInvert_0.5_const/alaska.3d.mod.smooth';

%open and read the file
fid = fopen(inputfile,'r');
C = textscan(fid,'%f %f %f %f');
fclose(fid);

%create interterpolation function
F = scatteredInterpolant(C{1},C{2},C{3},C{4});
F.Method = 'nearest';
F.ExtrapolationMethod = 'nearest';

%set the model output geometry
xvector = 189:0.5:237.0;
yvector = 56.0:0.5:73.0;
zvector = 0:1:50;

%generate the mesh
[xmesh,ymesh,zmesh] = meshgrid(xvector,yvector,zvector);

%determine velocities
vel = F(xmesh,ymesh,zmesh);

%smooth the matrix
nsx = 3;
nsy = 3;
nsz = 3;
vel = smooth_nxmxk_matrix(vel,nsx,nsy,nsz);

%write to file
fid = fopen(outputfile,'w');
for i = 1:length(xvector)
    for j = 1:length(yvector)
        for k = 1:length(zvector)
            fprintf(fid,'%3.1f %3.1f %3.1f %3.5f\n',xmesh(j,i,k),ymesh(j,i,k),zmesh(j,i,k),vel(j,i,k));
        end
    end
end

