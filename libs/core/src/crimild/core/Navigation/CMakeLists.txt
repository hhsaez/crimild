target_sources(
  crimild_core

  PRIVATE NavigationCell.cpp
  PUBLIC NavigationCell.hpp
  PRIVATE NavigationCellEdge.cpp
  PUBLIC NavigationCellEdge.hpp
  PRIVATE NavigationController.cpp
  PUBLIC NavigationController.hpp
  PRIVATE NavigationMesh.cpp
  PUBLIC NavigationMesh.hpp
  PRIVATE NavigationMeshContainer.cpp
  PUBLIC NavigationMeshContainer.hpp
  PRIVATE NavigationMeshOBJ.cpp
  PUBLIC NavigationMeshOBJ.hpp
)
