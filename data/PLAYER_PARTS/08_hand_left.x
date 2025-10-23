xof 0302txt 0064
template Header {
 <3D82AB43-62DA-11cf-AB39-0020AF71E433>
 WORD major;
 WORD minor;
 DWORD flags;
}

template Vector {
 <3D82AB5E-62DA-11cf-AB39-0020AF71E433>
 FLOAT x;
 FLOAT y;
 FLOAT z;
}

template Coords2d {
 <F6F23F44-7686-11cf-8F52-0040333594A3>
 FLOAT u;
 FLOAT v;
}

template Matrix4x4 {
 <F6F23F45-7686-11cf-8F52-0040333594A3>
 array FLOAT matrix[16];
}

template ColorRGBA {
 <35FF44E0-6C7C-11cf-8F52-0040333594A3>
 FLOAT red;
 FLOAT green;
 FLOAT blue;
 FLOAT alpha;
}

template ColorRGB {
 <D3E16E81-7835-11cf-8F52-0040333594A3>
 FLOAT red;
 FLOAT green;
 FLOAT blue;
}

template IndexedColor {
 <1630B820-7842-11cf-8F52-0040333594A3>
 DWORD index;
 ColorRGBA indexColor;
}

template Boolean {
 <4885AE61-78E8-11cf-8F52-0040333594A3>
 WORD truefalse;
}

template Boolean2d {
 <4885AE63-78E8-11cf-8F52-0040333594A3>
 Boolean u;
 Boolean v;
}

template MaterialWrap {
 <4885AE60-78E8-11cf-8F52-0040333594A3>
 Boolean u;
 Boolean v;
}

template TextureFilename {
 <A42790E1-7810-11cf-8F52-0040333594A3>
 STRING filename;
}

template Material {
 <3D82AB4D-62DA-11cf-AB39-0020AF71E433>
 ColorRGBA faceColor;
 FLOAT power;
 ColorRGB specularColor;
 ColorRGB emissiveColor;
 [...]
}

template MeshFace {
 <3D82AB5F-62DA-11cf-AB39-0020AF71E433>
 DWORD nFaceVertexIndices;
 array DWORD faceVertexIndices[nFaceVertexIndices];
}

template MeshFaceWraps {
 <4885AE62-78E8-11cf-8F52-0040333594A3>
 DWORD nFaceWrapValues;
 Boolean2d faceWrapValues;
}

template MeshTextureCoords {
 <F6F23F40-7686-11cf-8F52-0040333594A3>
 DWORD nTextureCoords;
 array Coords2d textureCoords[nTextureCoords];
}

template MeshMaterialList {
 <F6F23F42-7686-11cf-8F52-0040333594A3>
 DWORD nMaterials;
 DWORD nFaceIndexes;
 array DWORD faceIndexes[nFaceIndexes];
 [Material]
}

template MeshNormals {
 <F6F23F43-7686-11cf-8F52-0040333594A3>
 DWORD nNormals;
 array Vector normals[nNormals];
 DWORD nFaceNormals;
 array MeshFace faceNormals[nFaceNormals];
}

template MeshVertexColors {
 <1630B821-7842-11cf-8F52-0040333594A3>
 DWORD nVertexColors;
 array IndexedColor vertexColors[nVertexColors];
}

template Mesh {
 <3D82AB44-62DA-11cf-AB39-0020AF71E433>
 DWORD nVertices;
 array Vector vertices[nVertices];
 DWORD nFaces;
 array MeshFace faces[nFaces];
 [...]
}

Header{
1;
0;
1;
}

Mesh {
 18;
 2.75737;-1.39284;-0.93651;,
 -0.36423;-1.63333;-1.70975;,
 0.03932;0.58694;-1.70975;,
 3.03792;0.62400;-1.29572;,
 2.70558;1.62605;-1.27414;,
 -0.29301;1.58899;-1.68817;,
 -0.07388;1.46699;1.73132;,
 2.70558;1.62605;1.31730;,
 3.03792;0.62400;1.29572;,
 0.25846;0.46494;1.70975;,
 -0.53286;-1.63333;1.70975;,
 2.75737;-1.39284;0.93651;,
 -0.36423;-1.63333;-1.70975;,
 2.75737;-1.39284;-0.93651;,
 -0.53286;-1.63333;1.70975;,
 0.25846;0.46494;1.70975;,
 2.75737;-1.39284;0.93651;,
 3.03792;0.62400;1.29572;;
 
 10;
 4;0,1,2,3;,
 4;4,5,6,7;,
 4;8,9,10,11;,
 4;11,10,12,13;,
 4;1,14,15,2;,
 4;16,0,3,17;,
 4;3,2,5,4;,
 4;2,9,6,5;,
 4;9,8,7,6;,
 4;8,3,4,7;;
 
 MeshMaterialList {
  1;
  10;
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0;;
  Material {
   0.032000;0.032000;0.032000;1.000000;;
   5.000000;
   0.000000;0.000000;0.000000;;
   0.000000;0.000000;0.000000;;
  }
 }
 MeshNormals {
  14;
  0.190782;-0.118658;-0.974434;,
  0.163996;-0.026310;-0.986110;,
  0.171109;-0.057446;0.983576;,
  0.195117;-0.141494;0.970520;,
  -0.037293;0.999135;0.018389;,
  0.075280;-0.997161;0.001818;,
  -0.963632;0.266980;0.011653;,
  -0.999169;-0.025296;0.031958;,
  0.990463;-0.137777;-0.000000;,
  0.995861;0.090888;0.000000;,
  0.135672;0.066285;-0.988534;,
  -0.947679;-0.315368;0.049479;,
  0.145765;0.027051;0.988949;,
  0.949159;0.314798;0.000000;;
  10;
  4;0,0,1,1;,
  4;4,4,4,4;,
  4;2,2,3,3;,
  4;5,5,5,5;,
  4;6,6,7,7;,
  4;8,8,9,9;,
  4;1,1,10,10;,
  4;7,7,11,11;,
  4;2,2,12,12;,
  4;9,9,13,13;;
 }
 MeshTextureCoords {
  18;
  0.375000;0.000000;,
  0.625000;0.000000;,
  0.625000;0.250000;,
  0.375000;0.250000;,
  0.375000;0.250000;,
  0.625000;0.250000;,
  0.625000;0.500000;,
  0.375000;0.500000;,
  0.375000;0.500000;,
  0.625000;0.500000;,
  0.625000;0.750000;,
  0.375000;0.750000;,
  0.625000;1.000000;,
  0.375000;1.000000;,
  0.875000;0.000000;,
  0.875000;0.250000;,
  0.125000;0.000000;,
  0.125000;0.250000;;
 }
}
