


#include "BaseLib/Graphics/Material.h"



namespace mg
{

Texture::Texture()
{
	dim_ = 0;
	data_ = 0;
}

Texture::~Texture()
{
	delete[] data_;
	data_ = 0;
}

PhongMaterial::PhongMaterial()
{
}

PhongMaterial::~PhongMaterial()
{
}


};








