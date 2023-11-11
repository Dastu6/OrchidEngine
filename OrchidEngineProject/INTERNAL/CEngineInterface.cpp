#include "CEngineInterface.h"

CEngineInterface::CEngineInterface(CEngine &engine) {
    bEGIFullscreen = false;
    bEGIWireframeChecked = false;
    bEGIFPSPlotChecked = false;
    iEGIFpsLimiter = 85;
    iEGIWidth = engine.uiENGWidth;
    iEGIHeight = engine.uiENGHeight;
    gfEGIBrightness = engine.gfENGGetBrightness();
    gfEGIContrast = engine.gfENGGetContrast();
    gfEGISaturation = engine.gfENGGetSaturation();
    gfEGIGamma = engine.gfENGGetGamma();
    bEGINormeRec_709 = engine.bENGGetNormRec();
    bEGICreateEntitySubModule = true; //A remettre � false
    //New entity values
    pgfEGINewEntityXYZPos = new GLfloat[3];
    pgfEGINewEntityXYZPos[0] = 0.f;
    pgfEGINewEntityXYZPos[1] = 0.f;
    pgfEGINewEntityXYZPos[2] = 0.f;
    uiEGINewEntityID = 1;
    gfEGINewEntityScaleRatio = 1.f;
    //New entity material values
    vec3EGINewEntityAmbient = glm::vec3(1.0f, 1.0f, 1.0f);
    vec3EGINewEntityDiffuse = glm::vec3(1.0f, 1.0f, 1.0f);
    vec3EGINewEntitySpecular = glm::vec3(1.0f, 1.0f, 1.0f);
    fEGINewEntityShininess = 1.0f;
    fEGINewEntityTransparency = 1.0f;

    
    vec3EGITestLightColor = engine.vec3ENGTestLightColor;
    vec3EGITestLightPosition = engine.vec3ENGTestLightPosition;
    gfEGITestLightAmbientIntensity = engine.gfENGTestLightAmbientIntensity;
    gfEGITestLightDiffuseStrength = engine.gfENGTestLightDiffuseStrength;
    gfEGITestLightSpecularStrength = engine.gfENGTestLightSpecularStrength;

    pcEGINewEntityVertexShaderName = "core.vs";
    pcEGINewEntityFragmentShaderName = "core.frag";
    iEGITextureNumber = 0;
    rdrEGIRender = CRender();

    //Launch commands for ImGui and ImPlot
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImPlot::CreateContext();

    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(engine.pwindowENGWindow, true);
    ImGui_ImplOpenGL3_Init("#version 330");
}

CEngineInterface::~CEngineInterface() {
    //EXIT IMGUI
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImPlot::DestroyContext();
    ImGui::DestroyContext();
}

///////////////////////////////// IMGUI MODULES /////////////////////////////////

//Interface module related to engine settings
void CEngineInterface::EGIEngineModule(CEngine &engine) {
    ImGui::Begin("Engine Settings");
    ImGui::Columns(2);
    ImGui::Text("Engine settings");
    ImGui::Checkbox("Fullscreen", &bEGIFullscreen);
    ImGui::Text("Width");
    ImGui::SameLine();
    ImGui::SliderInt("w", &iEGIWidth, 640, 1920);
    ImGui::Text("Height");
    ImGui::SameLine();
    ImGui::SliderInt("h", &iEGIHeight, 480, 1080);
    ImGui::SameLine();
    if (ImGui::SmallButton("Apply new resolution")) {
        engine.ENGChangeResolution((GLuint)iEGIWidth, (GLuint)iEGIHeight);
        std::cout << "Nouvelle r�solution";
    }
    std::string FPS = std::to_string(engine.gfENGFpsCounter);
    std::string ms = std::to_string(engine.gfENGFrameDelayMS);
    std::string FPS_MS_TEXT = "Frame " + std::to_string(engine.iENGFrameNumber) + "\nFPS : " + std::to_string(engine.gfENGFpsCounter) + " / FrameTime : " + std::to_string(engine.gfENGFrameDelayMS) + " ms";
    ImGui::Text(FPS_MS_TEXT.c_str());
    //ImGui::Columns(2); //Permet de cr�er 2 colonnes dans la m�me interface et potentiellement de faire des sous-menus
    ImGui::Text("Frame Limite");
    ImGui::SameLine();
    ImGui::SliderInt("fpslim", &iEGIFpsLimiter, 1, 400); //"##" <=> label vide
    ImGui::Checkbox("FPS Plot display", &bEGIFPSPlotChecked);

    //Frame Stats Plot
    if (bEGIFPSPlotChecked == true) {
        if (ImPlot::BeginPlot("FPS Plot")) {
            ImPlot::SetupAxes("t(s)", "FPS");
            ImPlot::PlotLine("FPS", engine.pgfENGFrameDelayMSBuffer, engine.pgfENGFpsCounterBuffer, 1001);
            ImPlot::SetNextMarkerStyle(ImPlotMarker_Circle);
            ImPlot::EndPlot();
        }
    }
    ImGui::ColorPicker3("Light color vec", (float*)&vec3EGITestLightColor);
    ImGui::SliderFloat("Light ambient intensity", &gfEGITestLightAmbientIntensity, 0.f, 1.f);
    ImGui::SliderFloat("Light diffuse strength", &gfEGITestLightDiffuseStrength, 0.f, 1.f);
    ImGui::SliderFloat("Light specular strength", &gfEGITestLightSpecularStrength, 0.f, 256.f);
    ImGui::SliderFloat3("Light position", (float*)&vec3EGITestLightPosition, -10.f, 10.f);

    ImGui::ColorEdit4("Background color", engine.pgfENGBackgroundColor);
    ImGui::Checkbox("Wireframe display", &bEGIWireframeChecked);

    ImGui::NextColumn();
    ImGui::Text("Post-processing tools");
    ImGui::SliderFloat("Brightness", &gfEGIBrightness, 0.0f, 1.0f);
    ImGui::SliderFloat("Contrast", &gfEGIContrast, 0.0f, 4.0f);
    ImGui::Checkbox("Norme Rec. 709", &bEGINormeRec_709);
    ImGui::SliderFloat("Saturation", &gfEGISaturation, -20.0f, 20.0f);
    ImGui::SliderFloat("Gamma", &gfEGIGamma, 0.0f, 4.0f);
    if (ImGui::SmallButton("Reset to default values")) {
        gfEGIBrightness = 0.0f;
        gfEGIContrast = 1.0f;
        gfEGISaturation = 1.0f;
        gfEGIGamma = 1.0f;
    }
    ImGui::End();
}

//Interface for inputs
void CEngineInterface::EGIInputsModule(CEngine &engine) {

}

//Interface for textures
void CEngineInterface::EGITexturesModule(CEngine &engine) {
    ImGui::SetNextWindowSizeConstraints(ImVec2(SIZE_TEXTURE_INTERFACE, 0), ImVec2(engine.iENGScreenWidth, engine.iENGScreenHeight));
    ImGui::Begin("Textures");
    int nombre_texture_par_ligne = ImGui::GetWindowSize()[0]/SIZE_TEXTURE_INTERFACE;
    ImGui::BeginChild("Textures panel", ImVec2(0, 0), true);
    for (int boucle_tex = 0; boucle_tex < engine.uiENGNumberOfTexturesFile; boucle_tex++) {
        //On doit cast le numero de texture GLuint vers (void*)(intptr_t) car ImGui demande un ImTextureId et vu que c'est propre � chaque API
        //Il faut bien cast notre valeur (cela aurait �t� diff�rent avec DX9 ou Vulkan)
        // Fonction pour charger l'affichage d'une image dans un module ImGui ci-dessous
        //ImGui::Image((void*)(intptr_t)textureImage.guiTEXGetNumeroTexture(), ImVec2(textureImage.iTEXGetTextureWidth() / 5, textureImage.iTEXGetTextureHeight() / 5));
        CTexture textureImage = engine.ptexENGAllTextures[boucle_tex];
        if (boucle_tex % nombre_texture_par_ligne != 0) {
            ImGui::SameLine();
        }
        if (ImGui::ImageButton((void*)(intptr_t)textureImage.guiTEXGetNumeroTexture(), ImVec2(SIZE_TEXTURE_INTERFACE, SIZE_TEXTURE_INTERFACE))) {
            iEGITextureNumber = boucle_tex;
        }
    }
    ImGui::EndChild();
    ImGui::End();
}

//Interface module related to entities 
void CEngineInterface::EGIEntitiesModule(CEngine &engine) {
    ImGui::Begin("Entities");
    static int selectedEntity = 0;

    ImGui::Columns(3);
    ImGui::Checkbox("New entity", &bEGICreateEntitySubModule);

    ImGui::BeginChild("Entity panel", ImVec2(180, 0), true);
    for (int nb_ent = 0; nb_ent < engine.iENGGetNumberOfEntities(); nb_ent++) {
        // FIXME: Good candidate to use ImGuiSelectableFlags_SelectOnNav
        char label[128];
        sprintf_s(label, engine.pentENGCubeEntitiesList[nb_ent].strENTName.c_str(), nb_ent);
        if (ImGui::Selectable(label, selectedEntity == nb_ent))
            selectedEntity = nb_ent;
    }
    ImGui::EndChild();

    if (bEGICreateEntitySubModule == true) {
        ImGui::NextColumn();
        static int entityTypeCombo;
        const char* entityItems[] = { "Cube", "Light" };
        ImGui::Combo("Entity type", &entityTypeCombo, "Cube\0" "Light\0");
        //Sliders for XYZ Axis position of the new entity
        const char* axisSliders[] = { "X", "Y", "Z" };
        for (int boucle_axe = 0; boucle_axe < 3; boucle_axe++) {
            std::string axisText = axisSliders[boucle_axe];
            axisText += " Axis";
            ImGui::SliderFloat(axisText.c_str(), &pgfEGINewEntityXYZPos[boucle_axe], -100.0f, 100.0f); //On peut mettre -1000 1000
            ImGui::SameLine();
            std::string axisRound = axisSliders[boucle_axe];
            axisRound += " Round value";
            if (ImGui::SmallButton(axisRound.c_str())) {
                pgfEGINewEntityXYZPos[boucle_axe] = (int)round(pgfEGINewEntityXYZPos[boucle_axe]);
            }
            ImGui::NewLine();
            int button_values[6] = { -100,-10,-1,1,10,100 };
            for (int boucle_button = 0; boucle_button < 6; boucle_button++) {
                ImGui::SameLine();
                std::string buttonTextValue = std::to_string(button_values[boucle_button]) + " ";
                buttonTextValue += axisSliders[boucle_axe];
                if (ImGui::SmallButton(buttonTextValue.c_str())) {
                    pgfEGINewEntityXYZPos[boucle_axe] += button_values[boucle_button];
                }
            }
        }
        ImGui::SliderFloat("Scale ratio", &gfEGINewEntityScaleRatio, 0.0f, 100.0f);
        //Entity's material values
        ImGui::SliderFloat3("Entity's ambient", (float*)&vec3EGINewEntityAmbient, 0.0f, 1.0f);
        ImGui::SliderFloat3("Entity's diffuse", (float*)&vec3EGINewEntityDiffuse, 0.0f, 1.0f);
        ImGui::SliderFloat3("Entity's specular", (float*)&vec3EGINewEntitySpecular, 0.0f, 1.0f);
        ImGui::SliderFloat("Entity's shininess", &fEGINewEntityShininess, 0.0f, 1.0f);
        ImGui::SliderFloat("Entity's transparency", &fEGINewEntityTransparency, 0.0f, 1.0f);

        //If the user want to create a new light
        if (entityTypeCombo == 1) {
            ImGui::ColorPicker3("Color's light", pgfEGINewLightColor);
            ImGui::SliderFloat("Ambient Intensity", &gfEGINewLightAmbientIntensity, 0.f, 1.f);
        }
        
        //ImStrncpy(newEntityTexturePath, "../../../Assets/", sizeof("../../../Assets/"));
        char newEntityTexturePath[512] = "../../../Assets/";
        ImGui::InputText("Texture PATH", newEntityTexturePath, sizeof(newEntityTexturePath));
        if (ImGui::SmallButton("Create entity")) {
            //Create a new entity with the set parameters and reset values to default ones
            entity_type_enum newEntityType;
            unsigned int newEntityId = engine.uiENGGetNextFreeEntityID();
            glm::vec3 newEntityWorldPosition(pgfEGINewEntityXYZPos[0], pgfEGINewEntityXYZPos[1], pgfEGINewEntityXYZPos[2]);
            if (entityTypeCombo == 0) {
                newEntityType = cube;
                CEntity newEntity = CEntity(newEntityType, newEntityId, newEntityWorldPosition, "core.vs", "core.frag", iEGITextureNumber, vec3EGINewEntityAmbient, vec3EGINewEntityDiffuse, vec3EGINewEntitySpecular, fEGINewEntityShininess, fEGINewEntityTransparency);
                newEntity.ENTChangeWorldPosition(newEntity.vec3ENTWorldPosition);
                newEntity.ENTScaleEntitySize(gfEGINewEntityScaleRatio);
                rdrEGIRender.RDRCreateMandatoryForEntity(engine, newEntity, newEntity.uiENTId);
                engine.ENGAddCubeEntity(newEntity); //Modifier pour ajouter dans les listes correspondantes (s�parer light et cube par ex.)
            }
            if (entityTypeCombo == 1) {
                newEntityType = light;
            }
        }
    }
    ImGui::NextColumn();
    //Display informations about selected entity : ID, Position X,Y,Z et texture pour l'instant
    if (selectedEntity == 0) {
        ImGui::Text("A");
    }
    if (selectedEntity == 1) {
        ImGui::Text("B");
    }
    ImGui::SameLine();
    ImGui::End();
}

void CEngineInterface::EGICameraModule(CEngine& engine, CCamera& camera) {

}

////////////////////////////////////////////////////////////////////////////////

void CEngineInterface::EGIWireframeUpdate() {
    if (bEGIWireframeChecked == false) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }
    else if (bEGIWireframeChecked == true) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }
}

//Check if the render mode should be windowed or fullscreen
void CEngineInterface::EGIFullscreenUpdate(CEngine &engine) {
    if (bEGIFullscreen == false) {
        glfwSetWindowMonitor(engine.pwindowENGWindow, NULL, 60, 60, engine.uiENGWidth, engine.uiENGHeight, 0);
    }
    else if (bEGIFullscreen == true) {
        glfwSetWindowMonitor(engine.pwindowENGWindow, engine.pmonitorENGMonitor, 0, 0, engine.uiENGWidth, engine.uiENGHeight, 0);
    }
}

//Fonction � compl�ter notamment avec les wireframe et le fullscreen car les laisser dans l'interface est moins logique
void CEngineInterface::EGIInterfaceToEngine(CEngine &engine) {
    engine.ENGSetFpsLimit(iEGIFpsLimiter);
    engine.ENGSetBrightness(gfEGIBrightness);
    engine.ENGSetContrast(gfEGIContrast);
    engine.ENGSetSaturation(gfEGISaturation);
    engine.ENGSetGamma(gfEGIGamma);
    engine.ENGSetNormRec(bEGINormeRec_709);
    engine.vec3ENGTestLightColor = vec3EGITestLightColor;
    engine.gfENGTestLightAmbientIntensity = gfEGITestLightAmbientIntensity;
    engine.gfENGTestLightDiffuseStrength = gfEGITestLightDiffuseStrength;
    engine.gfENGTestLightSpecularStrength = gfEGITestLightSpecularStrength;
    engine.vec3ENGTestLightPosition = vec3EGITestLightPosition;
}

void CEngineInterface::EGIUpdate(CEngine &engine) { //Update the display of the interface at each frame time
    //Pre-Update
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    //Interface Modules rendering methods
    EGIEngineModule(engine);
    EGIInputsModule(engine);
    EGITexturesModule(engine);
    EGIEntitiesModule(engine);
    EGICameraModule(engine, engine.inpENGInputs.camINPChosenCamera);

    EGIWireframeUpdate();
    EGIFullscreenUpdate(engine);

    EGIInterfaceToEngine(engine);

    //Post Update
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}