// Ashikmin BRDF model
static const float INV_PI = 0.3183098861;
static const float PI_23  = 72.256631032;
static const float PI_8   = 25.132741228;

static const float3 NeutralNormal = float3(0.5, 0.5, 1.0);
static const float3 corneaTransparency = float3(0.97, 0.92, 0.838045);

// Determine Thickness of irisSize-Layer
static const float texDepth = 0.07; // value 1 of texture * texDepth = mm	
static const float thicknessABL = texDepth * 0.2 * 0.12;

// Attenuation Coefficients of Eumelanin and Pheomelanin
static const float3 layerAttE = float3(17.6, 59.6, 121.3);
static const float3 layerAttP = float3(1.4464, 8.2264, 22.7017);

// Scattering Coefficient of Stroma-Layer
static const float3 layerScattStroma = float3(4.13, 6.7, 10.97);

// BRDF-loop stepsize
static const float s = 1.0/16.0;

float2 ASNganBRDF(
	float3 n,
	float3 v,
	float3 l,
	float3 diffuse,
	float exponent,
	float fresnel) 
{
   l = normalize(l);
   v = normalize(v);
   
   const float  r = (diffuse.x+diffuse.y+diffuse.z)*0.333333;
   const float3 h = normalize(l-v);
   //const float3 vn = float3(v.x/20.0, v.y/20.0, v.z/20.0);// + float3(0.0, 20.0, 0.0);
   const float normalization = (exponent+1.0)/PI_8;

   const float VDotH = max(dot(-v,h),0.0);
   const float NDotH = max(dot(n,h),0.0);
   const float NDotV = max(dot(n,-v),0.0);
   const float NDotL = max(dot(n,l),0.0);
   //const float VDotH = max(dot(normalize(-v),normalize(h)),0.0);
   return float2(
	   max((pow(NDotH,exponent)/(VDotH*max(NDotL,NDotV)))*normalization*(fresnel+(1.0-fresnel)*pow(VDotH,5.0)), 0.0),
	   max(((28.0*r)/PI_23)*(1.0-fresnel)*(1.0-pow(1.0-(NDotL/2.0),5.0))*(1.0-pow(1.0-(NDotV/2.0),5.0)), 0.0) ) * NDotL;
}

// Rotate Point
float3 rotatePoint (float rotationX, float rotationY, float3 point) {
	// create rotation matrix
	float3x3 rotationXMatrix = float3x3(float3(1.0, 0.0, 0.0), float3(0.0, cos(rotationX), -sin(rotationX)), float3(0.0, sin(rotationX), cos(rotationX)));
	float3x3 rotationYMatrix = float3x3(float3(cos(rotationY), 0.0, sin(rotationY)), float3(0.0, 1.0, 0.0), float3(-sin(rotationY), 0.0, cos(rotationY)));
	float3x3 rotation = mul(rotationXMatrix, rotationYMatrix);
	
	return mul(rotation, point);	
}

// Expand a range-compressed vector
float3 expand(float3 v) {
	return (v - 0.5) * 2;
}

// Modify Texture-Coordinates to simulate pupilSize-Variation
float2 pupilVariation(bool inIris, float pupilDistance, float irisScaleraBorder, float pupilSize, float2 eyeDir) {
	float2 coord;
	float l;
	if(inIris) {
		l = pupilDistance / (irisScaleraBorder) * (0.5 + pupilSize) - pupilSize;
		l = max(l,0);
	}
	else
		l = 0.5 + (pupilDistance-irisScaleraBorder) / (1.0-irisScaleraBorder) * 0.5;

	return (0.5 * l * normalize(eyeDir) + 0.5);
}

// Calculate Line-Point-Intersection
float2 pointLineIntersection (float2 p1, float2 p2, float2 p3) {
	float2 u = normalize(p2 - p1);
	float Lambda = (p3.x - p1.x) * u.x + (p3.y - p1.y) * u.y;
	float2 D = p1 + Lambda * u;
	return D;
}

// Calculate Distance and Orientation to Eyelid
float4 distanceEyeLid(
	float2[] positions,
	float2 transformedModelPos,
	float occlusionStretchX,
	float occlusionMoveX,
	float occlusionStretchY,
	float occlusionMoveY)
{
	float eyelid_distance = 2.0;
	float2 eyelid_orientation;
	
	// adapt Eyelid-Occlusion
	for (int i = 0; i < 16; i ++) {
		positions[i].x = expand(positions[i].x) * occlusionStretchX + occlusionMoveX;
		positions[i].y = expand(positions[i].y) * occlusionStretchY + occlusionMoveY;
	}
	
	float2 p1, p2, D;
	// calculate Distances
	for (int i = 0; i < 15; i ++) {
		p1 = float2(positions[i].x, positions[i].y);
		p2 = float2(positions[i + 1].x, positions[i + 1].y);

		D = pointLineIntersection(p1, p2, transformedModelPos);
		
		if (D.x > p2.x)
			D = p2;
		if (D.x < p1.x)
			D = p1;
		if (length(D - transformedModelPos) < eyelid_distance) {
			eyelid_distance = distance(D, transformedModelPos);
			eyelid_orientation = normalize(D - transformedModelPos);
		}		
	}
	
	// determine if current Fragment is higher or lower than the EyeLid
    float linearBlend, yLid, topEye;
	topEye = 0.0;	
	if (transformedModelPos.x < positions[15].x && transformedModelPos.x > positions[0].x) {
		for (int i = 0; i < 15; i++) {
			if (transformedModelPos.x < positions[i + 1].x) {
				// linear interpolation of Eyelid-Points
				linearBlend = (transformedModelPos.x - positions[i].x) / (positions[i + 1].x - positions[i].x);
				yLid = positions[i].y * (1.0 - linearBlend) + positions[i + 1].y * linearBlend;
				if (transformedModelPos.y < yLid)
					topEye = 1.0;
				break;
			}
		}
	}
	
	return float4(eyelid_distance, eyelid_orientation, topEye);
}

// Calculate Occlusion of Eyelid (takes care on BRDF-Model)
float3 brdfShadow(float3 modelLight, float3 p, float2 eyelid_direction, float eyelid_distance) {
	float2 D = pointLineIntersection(0.0, eyelid_direction, modelLight.xy);
	if (eyelid_direction.x < 0.0) {
		if (D.x > 0.0 || D.x < eyelid_direction.x)
			D = 0.0;
	}
	else {
		if (D.x < 0.0 || D.x > eyelid_direction.x)
			D = 0.0;
	}
	return p * (1.0 - length(D) * (1.0 - eyelid_distance));
}

// Refract incoming Light-Rays on the Cornea (Darkening on irisSize-Edge)
float3 refractLight(
	float blendLight,
	float3 l,
	float3 modelLight,
	float2 eyeDir,
	float irisScaleraBorder,
	sampler2D lightRefractionMap,
	float3x3 viewMatrix)
{
	modelLight = normalize(modelLight);
	
	// z-Value of 3-Dimensional Look-Up-Texture (discrete Distance from pupilSize)
	const int pupilDistance = int(min((length(eyeDir)) * 2.2, 1.0) * 11.66);
	
	// Determine x-Value of 3-Dimensional Look-Up-Texture (Gradient of incoming Light in Model-Space)
	const float parallelLightComponent = sqrt(1.0 - modelLight.z * modelLight.z);
	const float lightGradient = atan(modelLight.z / parallelLightComponent) / 1.5708;
	const float2 normEyeDir = normalize(-eyeDir);
	
	// Calculate y-Value of 3-Dimensional Look-Up-Texture (Angle of incoming Light compared to radius)
	float lightAngle = acos(dot(normEyeDir, modelLight.xy / parallelLightComponent))/(6.2831);
	if ((modelLight.y / modelLight.x) * eyeDir.x > eyeDir.y)
		lightAngle = 1.0 - lightAngle;
	
	// Texture-Look-Up to determine refracted Light
	float3 lightRefr = tex2D(lightRefractionMap, float2(lightGradient * 0.064 + 0.00133 + pupilDistance * 0.0667, lightAngle * 0.99 + 0.005)).rgb;

	float3 lightDirection;
	lightDirection.z = (lightRefr.g - 0.5) * 10.0;
	lightDirection.xy = (lightRefr.r - 0.5) * 10.0;	
	
	// Translate from Radius-Space back to Model-Space
	lightDirection.xy *= normEyeDir;	
	float2 irisPosVer = float2(normEyeDir.y, -normEyeDir.x);
	lightDirection.xy += irisPosVer * (lightRefr.b - 0.5) * 10.0;
	
	const float lightIntensity = length(modelLight);
	
	// Linear blend Light at the Edge of the irisSize
	if (length(eyeDir) > irisScaleraBorder - blendLight) {
		float3 l_neu = mul((float3x3)viewMatrix, normalize(lightDirection) * lightIntensity);
		float blend = length(eyeDir) - (irisScaleraBorder - blendLight);
		blend /= blendLight;
		l = l_neu + (l - l_neu) * blend;
	}
	else {
		l = mul((float3x3)viewMatrix, normalize(lightDirection) * lightIntensity);
	}
	
	return float3(l);
}

