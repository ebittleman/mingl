
import sys
import trimesh
import xatlas


mesh = trimesh.load(sys.argv[1],force='mesh')

vmap, faces, uv = xatlas.parametrize(mesh.vertices, mesh.faces)

print(",\n".join(f"{rec[0]},{rec[1]}" for rec in uv))
