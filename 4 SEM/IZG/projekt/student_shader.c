/*!
 * @file 
 * @brief This file contains implemenation of phong vertex and fragment shader.
 *
 * @author Tomáš Milet, imilet@fit.vutbr.cz
 */

#include<math.h>
#include<assert.h>

#include"student/student_shader.h"
#include"student/gpu.h"
#include"student/uniforms.h"

/// \addtogroup shader_side Úkoly v shaderech
/// @{

void phong_vertexShader(
    GPUVertexShaderOutput     *const output,
    GPUVertexShaderInput const*const input ,
    GPU                        const gpu   ){
  /// \todo Naimplementujte vertex shader, který transformuje vstupní vrcholy do clip-space.<br>
  /// <b>Vstupy:</b><br>
  /// Vstupní vrchol by měl v nultém atributu obsahovat pozici vrcholu ve world-space (vec3) a v prvním
  /// atributu obsahovat normálu vrcholu ve world-space (vec3).<br>
  /// <b>Výstupy:</b><br>
  /// Výstupní vrchol by měl v nultém atributu obsahovat pozici vrcholu (vec3) ve world-space a v prvním
  /// atributu obsahovat normálu vrcholu ve world-space (vec3).
  /// Výstupní vrchol obsahuje pozici a normálu vrcholu proto, že chceme počítat osvětlení ve world-space ve fragment shaderu.<br>
  /// <b>Uniformy:</b><br>
  /// Vertex shader by měl pro transformaci využít uniformní proměnné obsahující view a projekční matici.
  /// View matici čtěte z uniformní proměnné "viewMatrix" a projekční matici čtěte z uniformní proměnné "projectionMatrix".
  /// Zachovejte jména uniformních proměnných a pozice vstupních a výstupních atributů.
  /// Pokud tak neučiníte, akceptační testy selžou.<br>
  /// <br>
  /// Využijte vektorové a maticové funkce.
  /// Nepředávajte si data do shaderu pomocí globálních proměnných.
  /// Pro získání dat atributů použijte příslušné funkce vs_interpret* definované v souboru program.h.
  /// Pro získání dat uniformních proměnných použijte příslušné funkce shader_interpretUniform* definované v souboru program.h.
  /// Vrchol v clip-space by měl být zapsán do proměnné gl_Position ve výstupní struktuře.<br>
  /// <b>Seznam funkcí, které jistě použijete</b>:
  ///  - gpu_getUniformsHandle()
  ///  - getUniformLocation()
  ///  - shader_interpretUniformAsMat4()
  ///  - vs_interpretInputVertexAttributeAsVec3()
  ///  - vs_interpretOutputVertexAttributeAsVec3()
  (void)output;
  (void)input;
  (void)gpu;

  //get handle to all uniforms
  Uniforms const uniformsHandle = gpu_getUniformsHandle(gpu);//gpu
  
  UniformLocation const viewMatrixLocation = getUniformLocation(gpu, //gpu handle
	  "viewMatrix");//name of uniform variable
  Mat4 const*const view = shader_interpretUniformAsMat4(
	  uniformsHandle, //handle to all uniforms
	  viewMatrixLocation);//location of view matrix

  UniformLocation const projectionMatrixLocation = getUniformLocation(
	  gpu, //gpu handle
	  "projectionMatrix");//name of uniform variable
  Mat4 const*const proj = shader_interpretUniformAsMat4(
	  uniformsHandle, //handle to all uniforms
	  projectionMatrixLocation);//location of projection matrix

  Vec3 const*const position = vs_interpretInputVertexAttributeAsVec3(gpu, input, 0);
  Vec3 const*const normal1 = vs_interpretInputVertexAttributeAsVec3(gpu, input, 1);
	  

  Mat4 mvp;
  multiply_Mat4_Mat4(&mvp, proj, view);

  Vec4 pos4;
  copy_Vec3Float_To_Vec4(&pos4, position, 1.f);
  multiply_Mat4_Vec4( &output->gl_Position, &mvp, &pos4);

  Vec3 *const pos = vs_interpretOutputVertexAttributeAsVec3( gpu, output, 0);
  copy_Vec3(pos, position);

  Vec3 *const nor = vs_interpretOutputVertexAttributeAsVec3( gpu, output, 1);
  copy_Vec3(nor, normal1);


}

void phong_fragmentShader(
    GPUFragmentShaderOutput     *const output,
    GPUFragmentShaderInput const*const input ,
    GPU                          const gpu   ){
  /// \todo Naimplementujte fragment shader, který počítá phongův osvětlovací model s phongovým stínováním.<br>
  /// <b>Vstup:</b><br>
  /// Vstupní fragment by měl v nultém fragment atributu obsahovat interpolovanou pozici ve world-space a v prvním
  /// fragment atributu obsahovat interpolovanou normálu ve world-space.<br>
  /// <b>Výstup:</b><br> 
  /// Barvu zapište do proměnné color ve výstupní struktuře.<br>
  /// <b>Uniformy:</b><br>
  /// Pozici kamery přečtěte z uniformní proměnné "cameraPosition" a pozici světla přečtěte z uniformní proměnné "lightPosition".
  /// Zachovejte jména uniformních proměnný.
  /// Pokud tak neučiníte, akceptační testy selžou.<br>
  /// <br>
  /// Dejte si pozor na velikost normálového vektoru, při lineární interpolaci v rasterizaci může dojít ke zkrácení.
  /// Zapište barvu do proměnné color ve výstupní struktuře.
  /// Shininess faktor nastavte na 40.f
  /// Difuzní barvu materiálu nastavte na čistou zelenou.
  /// Spekulární barvu materiálu nastavte na čistou bílou.
  /// Barvu světla nastavte na bílou.
  /// Nepoužívejte ambientní světlo.<br>
  /// <b>Seznam funkcí, které jistě využijete</b>:
  ///  - shader_interpretUniformAsVec3()
  ///  - fs_interpretInputAttributeAsVec3()
  (void)output;
  (void)input;
  (void)gpu;

  Vec3 const* pos = fs_interpretInputAttributeAsVec3(gpu, input, 0);
  Vec3 const* nor = fs_interpretInputAttributeAsVec3(gpu, input, 1);

  Uniforms const uniformsHandle = gpu_getUniformsHandle(gpu);//gpu
  UniformLocation const viewCameraPosition = getUniformLocation(gpu, "cameraPosition");
  Vec3 const*const cameraPosition = shader_interpretUniformAsVec3(
	  uniformsHandle, //handle to all uniforms
	  viewCameraPosition);//location of view matrix
  UniformLocation const viewLightPosition = getUniformLocation(gpu, "lightPosition");
  Vec3 const*const lightPosition = shader_interpretUniformAsVec3(
	  uniformsHandle, //handle to all uniforms
	  viewLightPosition);//location of view matrix
  

  Vec3 dM, sM, dL, sL, Normal, unL, L, unV, V, R, colorAttribute;
  float dF,sF;

  init_Vec3(&dM, 0.f, 1.f, 0.f);  // zelene
  init_Vec3(&dL, 1.f, 1.f, 1.f); // bile
  init_Vec3(&sM, 1.f, 1.f, 1.f); 
  init_Vec3(&sL, 1.f, 1.f, 1.f); 
  normalize_Vec3(&Normal, nor);
  //init_Vec3(&unL, (float)(lightPosition->data[0] - pos->data[0]), (float)(lightPosition->data[1] - pos->data[1]), (float)(lightPosition->data[2] - pos->data[2]));
  sub_Vec3(&unL, lightPosition, pos);
  normalize_Vec3(&L, &unL);
  dF = dot_Vec3(&Normal, &L);
  if (dF < 0.f) {
	  dF = 0.f;
  }
  else if (dF > 1.f) {
	  dF = 1.f;
  }
  //init_Vec3(&unV, (float)(cameraPosition->data[0] - pos->data[0]), (float)(cameraPosition->data[1] - pos->data[1]), (float)(cameraPosition->data[2] - pos->data[2]));
  sub_Vec3(&unV, cameraPosition, pos);
  normalize_Vec3(&V, &unV);
  multiply_Vec3_Float(&V, &V, -1.f);
  reflect(&R, &V, &Normal);
  sF = dot_Vec3(&R, &L);
  if (sF < 0.f) {
	  sF = 0.f;
  }
  else if (sF > 1.f) {
	  sF = 1.f;
  }
  sF = (float)pow(sF, 40.f);
  init_Vec3(&colorAttribute,
	  (dF*dM.data[0] * dL.data[0]) + (sF*sM.data[0] * sL.data[0]),
	  (dF*dM.data[1] * dL.data[1]) + (sF*sM.data[1] * sL.data[1]),
	  (dF*dM.data[2] * dL.data[2]) + (sF*sM.data[2] * sL.data[2]));
  copy_Vec3Float_To_Vec4(&output->color, &colorAttribute, 1.f);//alpha
}

/// @}
