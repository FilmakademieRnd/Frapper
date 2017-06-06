/*
-----------------------------------------------------------------------------
This source file is part of FRAPPER
research.animationsinstitut.de
sourceforge.net/projects/frapper

Copyright (c) 2008-2016 Filmakademie Baden-Wuerttemberg, Institute of Animation 

This program is free software; you can redistribute it and/or modify it under
the terms of the GNU Lesser General Public License as published by the Free Software
Foundation; version 2.1 of the License.

This program is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License along with
this program; if not, write to the Free Software Foundation, Inc., 59 Temple
Place - Suite 330, Boston, MA 02111-1307, USA, or go to
http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html
-----------------------------------------------------------------------------
*/

//!
//! \file "AnimatableMeshHairNode.cpp"
//! \brief Interface file for AnimatableMeshHairNode class.
//!
//! \author     Felix Bucella <felix.bucella@filmakademie.de>
//! \author     Simon Spielmann <sspielma@filmakademie.de>
//! \author     Nils Zweiling <nils.zweiling@filmakademie.de>
//! \author     Markus Rapp <markus.rapp@filmakademie.de>
//! \version    2.0
//! \date       06.10.2014 (last updated)
//!


#ifndef ANIMATABLEMESHHAIRNODE_H
#define ANIMATABLEMESHHAIRNODE_H

#include "GeometryAnimationNode.h"

namespace AnimatableMeshHairNode {
using namespace Frapper;

//!
//! Class implementing Screen Space Ambient Occlusion operations.
//!
class AnimatableMeshHairNode : public GeometryAnimationNode
{

    Q_OBJECT
    ADD_INSTANCE_COUNTER

public: // constructors and destructors

    //!
    //! Constructor of the AnimatableMeshHairNode class.
    //!
    //! \param name The name for the new node.
    //! \param parameterRoot A copy of the parameter tree specific for the type of the node.
    //!
    AnimatableMeshHairNode ( const QString &name, ParameterGroup *parameterRoot );

    //!
    //! Destructor of the AnimatableMeshHairNode class.
    //!
    //! Defined virtual to guarantee that the destructor of a derived class
    //! will be called if the instance of the derived class is saved in a
    //! variable of its parent class type.
    //!
    virtual ~AnimatableMeshHairNode ();


private: // functions
	struct GuideHairIndexData
	{
		float rootTexturePosition;
		float strandSize;
	};

	//!
	//! Saves texture to a file
	//!
	//! \param texture The texture pointer.
	//! \param FileName Name of the created file
	//! \param pixelFormat Pixel format of the texture
	//!
	void saveTextureToFile(Ogre::TexturePtr texture, const Ogre::String& FileName, Ogre::PixelFormat pixelFormat = Ogre::PF_FLOAT32_RGBA);

	//!
	//! Box Muller Transformation
	//!
	//! \param var1 Result 1 of box muller transformation
	//! \param var2 Result 2 of box muller transformation
	//!
	void BoxMullerTransform(float& var1, float& var2);

	//!
	//! Procedural creation of curly hair deviations
	//!
	//! \param totalHairGuideVertexNumber Vertex number of all hair guides combined
	//! \param numberOfGuideHairs Number of used guide hairs
	//!
	void createCurlDeviations(int totalHairGuideVertexNumber, int numberOfGuideHairs);

	//!
	//! Procedural creation of hair strand deviations
	//!
	//! \param tessellatedMasterStrandLengthMax Maximum length of tessellated guide strand
	//!
	void createDeviantStrands(size_t tessellatedMasterStrandLengthMax);

	//!
	//! Strand length calculation for hair thinning
	//!
	void createStrandLengths();

	//!
	//! Random circular coordinates for use with single strand rendering of interpolated hair strands
	//!
	void createRandomCircularCoordinates();

	//!
	//! Creation of coordinate frames per guide hair vertex. One coordinate frame consists of 3 vectors
	//!
	//! \param hairStrands Vector of guide hair strands
	//!
	void createCoordinateFrames(std::vector<std::vector<Ogre::Vector3>> hairStrands);

	//!
	//! Rotate a vector
	//!
	//! note: this method is only valid when the axis is perpendicular to prevVec
	//!
	//! \param rotationAxis axis to rotate 
	//! \param theta angle of rotation
	//! \param prevVec input vector
	//! \param newVec result vector
	//!
	void rotateVector(const Ogre::Vector3& rotationAxis,float theta,const Ogre::Vector3& prevVec, Ogre::Vector3& newVec);
	
	//!
	//! Creation of texture which hold the maximum/angle between hair strands of one face.
	//!
	//! \param scalpSubMesh Pointer to scalp sub mesh
	//! \param hairStrands
	//!
	void createMaxDistanceAngleTexture(Ogre::SubMesh *scalpSubMesh, std::vector<std::vector<Ogre::Vector3>> hairStrands);

	//!
	//! Calculate maximum distance between those vectors
	//!
	//! \param v1 Input vector 1
	//! \param v2 Input vector 2
	//! \param v3 Input vector 3
	//!
	//! \return Maximum distance between those vectors
	//!
	float calculateMaximumDistance(Ogre::Vector3 v1, Ogre::Vector3 v2, Ogre::Vector3 v3);

	//!
	//! Calculate maximum angle between those vectors
	//!
	//! \param v1 Input vector 1
	//! \param v2 Input vector 2
	//! \param v3 Input vector 3
	//!
	//! \return Maximum angle between input vectors
	//!
	float calculateMaximumAngle(Ogre::Vector3 v1, Ogre::Vector3 v2, Ogre::Vector3 v3);

	//!
	//! Creation of texture, which holds scalp mesh normals
	//!
	//! \param normalList List of scalp mesh normals
	//! \param indexNormalList Index list of scalp mesh normals
	//!
	void createScalpMeshNormalTexture(std::list<Ogre::Vector3> normalList, std::list<Ogre::Vector3> indexNormalList);

	//!
	//! Creation of texture, which determines how the hair strand is bended. (Used to calculate detail tessellation factor at run-time)
	//!
	//! \param hairStrands Vector of guide hair strands
	//!
	void createHairStrandBendTexture(std::vector<std::vector<Ogre::Vector3>> hairStrands);

protected: //functions

	//!
	//! Loads the animation mesh from file.
	//!
	//! \return True if the animation mesh was successfully loaded, otherwise False.
	//!
	bool loadMesh ();

	//!
	//! Assign texture to texture slot.
	//!
	//! \param mat The material.
	//! \param texture Pointer to the texture.
	//! \param slot The texture slot id.
	//! \param bindingType Defines to which shader the texture unit is bind
	//!
	void setTexture( Ogre::MaterialPtr &mat, Ogre::TexturePtr texture, unsigned int slot, Ogre::TextureUnitState::BindingType bindingType );

private slots:

	//!
	//! Changes the hair shader parameters.
	//!
	//! Is called when a GUI parameter has changed.
	//!
	void changeShaderParameter();

	//!
	//! Changes the hair fragment shader parameters.
	//!
	//! Is called when a GUI parameter has changed.
	//!
	void changeFragmentShaderParameter();

	//!
	//! Sets the scenes shader color in fragment shader
	//!
	void changeFragmentShaderColor();

	//!
	//! Changes scalp mesh visibility
    //!
    //! Is called when a GUI parameter has changed.
    //!
	void changeScalpMeshVisibility();


	//!
	//! Changes guide hairs visibility
    //!
    //! Is called when a GUI parameter has changed.
    //!
	void changeGuideHairsVisibility();


	//!
	//! Changes thinning parameter
    //!
    //! Thinning parameter is changed in shader and for deviations texture.
    //!
	void changeThinningParameter();

	//!
	//! Changes hair lod dependent on a slider with a value between 0.0 and 2.0
	//!
	void changeHairLODParameter();

	//!
	//! Changes segment screen size parameter
	//!
	void changeSegmentScreenSizeParameter();

	//!
	//! Change m_bendAngleMeaningful
	//!
	void changeBendAngleMeaningfulParameter();

	//!
	//! Change m_bendAngleMaxTessellation
	//!
	void changeBendAngleMaxTessellationParameter();

	//!
	//! Change m_rootVertexToScalpVertexDistance
	//!
	void changeRootVertexToScalpVertexDistanceParameter();

	//!
	//! Change between manual LOD and screen space adaptive LOD
	//!
	void changeShaderLODParameter();

	//!
	//! Change reflect color variable of hair
	//!
	void changeReflectColor();

	//!
	//! Change reflect color variable of hair
	//!
	void changeReflectStrength();

	//!
	//! Change specular lobes color for R
	//!
	void changeSpecularLobesColorsR();

	//!
	//!	Change transmit color for hair
	//!
	void changeTransmitColor();
	
	//!
	//! Change transmit strength for hair
	//!
	void changeTransmitStrength();
	
	//!
	//! Change specular lobes color for TT
	//!
	void changeSpecularLobesColorsTT();

	//!
	//! Change scatter color for hair
	//!
	void changeScatterColor();

	//!
	//! Change scatter strength for hair
	//!
	void changeScatterStrength();

	//!
	//! Change specular lobes for TRT
	//!
	void changeSpecularLobesColorsTRT();

	//!
	//! Change diffuse color for hair
	//!
	void changeDiffuseColor();

	//!
	//! Change diffuse strength for hair
	//!
	void changeDiffuseStrength();

	//!
	//! Change diffuse color for hair, which combines strength and color
	//!
	void changeDiffuseColorStrengthHair();

	//!
	//! Change reflect angular shift of specular lob R
	//!
	void changeReflectShift();

	//!
	//! Change transmit angular shift of specular lob R
	//!
	void changeTransmitShift();

	//!
	//! Change scatter angular shift of specular lob R
	//!
	void changeScatterShift();

	//!
	//! Change initial position of hair style
	//!
	void changeInitialHairStylePosition();

private: // members

	//!
	//! Current LOD of strand width
	//!
	float m_strandWidthLOD;

	//!
	//! Tessellation factor for the number of generated hairs (1.0 to 64.0)
	//!
	float m_tessellationFactor;

	//!
	//! Maximum number of hair segments of one hair guide
	//!
	int m_NumberMaxOfHairSegments;

	//!
	//! Number of all vertices, which are used for all hair guides.
	//!
	int m_NumberOverallHairGuideVertices;

	//!
    //! Number Of guide strands
    //!
	int m_NumberOfGuideHairs;

	//!
    //! vertex coordinates of guide strands
    //!
	Ogre::TexturePtr m_hairCoordTex;

	//!
	//! tangents of each vertex of guide strands (calculated at start up)
	//!
	Ogre::TexturePtr m_hairTangentsTex;

	//!
    //! material of scalp
    //!
	Ogre::MaterialPtr m_grundMat;

	//!
	//! buffer for random generated barycentric strand coordinates
	//!
	Ogre::TexturePtr m_strandCoordinatesTex;

	//!
	//! buffer for guide strand indices
	//!
	Ogre::TexturePtr m_hairIndexTex;


	//!
	//! buffer for guide strand indices end
	//!
	Ogre::TexturePtr m_hairIndexEndTex;

	//!
	//! buffer for curl deviations
	//!
	Ogre::TexturePtr m_hairCurlDeviationTex;

	//!
	//! texture size of m_hairCurlDeviationTex texture
	//!
	float m_curlTextureSizeXY;

	//!
	//! buffer for curl deviations
	//!
	Ogre::TexturePtr m_hairDeviationsTex;

	//!
	//! texture size of m_hairDeviationTex texture
	//!
	float m_deviationsTextureSizeXY;

	//!
	//! buffer for strand length
	//!
	Ogre::TexturePtr m_strandLengthTex;

	//!
	//! maximal lengths of strands
	//!
	float* m_strandLength;

	//!
	//! for thinning of the hair
	//!
	float m_thinning;

	//!
	//! size of Interpolated hair variables. this is used for hair thickness, interpolation coordinates
	//!
	static const int m_numStrandVariables;

	//!
	//! name of hair mesh for empty vertex buffer
	//!
	Ogre::String m_hairMeshName;

	//!
	//! generated hair mesh with empty vertex buffer
	//!
	Ogre::MeshPtr m_hairMesh;

	//!
	//! entity of scalp mesh
	//!
	Ogre::SubEntity* m_scalpMeshSubEntity;

	//!
	//! Texture for random circular coordinates
	//!
	Ogre::TexturePtr m_randomCircularCoordinatesTex;

	//!
	//! Texture for coordinate frames
	//!
	Ogre::TexturePtr m_coordinateFramesTex;

	//!
	//! Texture size for coordinate frames
	//!
	float m_coordinateFramesTextureSizeXY;

	//!
	//! Texture, which holds the maximum distance and maximum angle between 3 guide hairs
	//!
	Ogre::TexturePtr m_maxDistanceAngleTex;

	//!
	//! Textures size of m_maxDistanceAngleTex
	//!
	float m_maxDistanceAngleTextureSizeXY;

	//!
	//! Texture with scalp normals
	//!
	Ogre::TexturePtr m_scalpNormalsTex;

	//!
	//! Textures size of m_scalpNormalsTex
	//!
	float m_scalpNormalsTextureSizeXY;

	//!
	//! Texture with scalp normals indices
	//!
	Ogre::TexturePtr m_scalpNormalIndicesTex;

	//!
	//! Textures size of m_scalpNormalsIndicesTex
	//!
	float m_scalpNormalIndicesTextureSizeXY;

	//!
	//! Texture with bending value for hair strands
	//!
	Ogre::TexturePtr m_strandBendTex;

	//!
	//! Angle between two hair segments for maximum tessellation (RAD)
	//!
	float m_bendAngleMaxTessellation;

	//!
	//! Angle between two hair segements, which is meaningful enough to apply tessellation (RAD)
	//!
	float m_bendAngleMeaningful;


	//!
	//! Maximum distance between hair strand root vertex and scalp vertex
	//!
	float m_rootVertexToScalpVertexDistance;

	//!
	//! Reflect color value for hair shading
	//!
	Ogre::ColourValue m_reflectColor;
	
	//!
	//! Reflect strength value for hair shading
	//!
	float m_reflectStrength;

	//!
	//! Transmit color value for hair shading
	//!
	Ogre::ColourValue m_transmitColor;

	//!
	//! Transmit strength value for hair shading
	//!
	float m_transmitStrength;

	//!
	//! Scatter color value for hair shading
	//!
	Ogre::ColourValue m_scatterColor;
	
	//!
	//! Scatter strength value for hair shading
	//!
	float m_scatterStrength;

	//!
	//! Diffuse color value for hair shading
	//!
	Ogre::ColourValue m_diffuseColor;
	
	//!
	//! Diffuse strength value for hair shading
	//!
	float m_diffuseStrength;

	//!
	//! Const value for pi * 180
	//!
	static const float PIG;

	//!
	//! Part of hair guide name that should be included in the submesh name of the hair guide.
	//!
	QString m_hairGuideName;

	//!
	//! Entity of hair
	//!
	Ogre::Entity* m_hairEntity;

	//!
	//! True if assigned bone was found in skeleton
	//!
	bool m_boneFound;
	
	//!
	//! Defines to which bone of the skeleton the hair style has to be assigned
	//!
	Ogre::String m_boneNameToAssignToHair;
};

} // namespace AnimatableMeshHairNode 

#endif
