#include "takoyaki.hpp"

#include "main_editor.hpp"
#include "takoyaki/visualizer/graphics/gl/render_command.hpp"
#include "takoyaki/visualizer/graphics/main_window.hpp"
#include "takoyaki/visualizer/graphics/renderer.hpp"
#include "takoyaki/visualizer/visualizer.hpp"
#include "widgets/camera.hpp"
#include "widgets/uniforms_menu.hpp"

namespace {
glm::vec2 vertices[3] = {
    {-1.0f, 3.0f},
    {3.0f, -1.0f},
    {-1.0f, -1.0f},
};

static const char* vertexShaderCode = R"(
#version 450
in vec2 vPos;
void main(){
	gl_Position = vec4(vPos, 1.0, 1.0);
}
)";

static const char* fragmentShaderCodeBegin = R"(
#version 450
#define TAKOYAKI
out vec4 frag_color;
uniform float iTime;
uniform vec2 iResolution;
uniform int iFrame;
uniform vec3 iCameraOrigin;
uniform vec3 iCameraTarget;
uniform vec2 iFFTLow;
uniform vec2 iFFTMid;
uniform vec2 iFFTHigh;
)";

static const char* fragmentShaderCodeEnd = R"(
void main(){
	mainImage(frag_color, gl_FragCoord.xy);
}
)";

static const char* templateShaderCode = R"(// Available uniforms
//#=============================#
//* uniform float iTime;        *
//* uniform vec2 iResolution;   *
//* uniform int iFrame;         *
//* uniform vec3 iCameraOrigin; *
//* uniform vec3 iCameraTarget; *
//* uniform vec2 iFFTLow;       *
//* uniform vec2 iFFTMid;       *
//* uniform vec2 iFFTHigh;      *
//#=============================#

void mainImage(out vec4 fragColor, in vec2 fragCoord) { 
    vec2 uv = (fragCoord*2.0 - iResolution.xy) / iResolution.y;

    vec3 col = vec3(0.0);
	col.rg = uv.xy;

    fragColor = vec4(col, 1.0);
}
)";

static const char* glslFileTypeFilter = "*.glsl";
}  // namespace

namespace ht {
Takoyaki::Takoyaki(MainWindow&       window,
                   Visualizer const& visualizer,
                   Renderer&         renderer,
                   FileWatcher&      fileWatcher,
                   MainEditor&       editor,
                   Camera&           camera,
                   UniformsMenu&     uniformsMenu)
    : mWindow(window)
    , mVisualizer(visualizer)
    , mRenderer(renderer)
    , mFileWatcher(fileWatcher)
    , mEditor(editor)
    , mCamera(camera)
    , mUniformsMenu(uniformsMenu) {
    SetupListeners();
    CreateVertexBuffer();
    CreateRenderTarget();
}

Takoyaki::~Takoyaki() {}

void Takoyaki::Tick() {
    static float time     = static_cast<float>(glfwGetTime());
    static float demoTime = 0.0f;
    static int   frame    = 0;
    ++frame;

    const float deltaTime = static_cast<float>(glfwGetTime()) - time;
    time                  = static_cast<float>(glfwGetTime());
    mCurrentTime += deltaTime;
    demoTime += deltaTime;

    mEditor.Update(deltaTime, !mCurrentProject.empty(), *mRenderTarget);

    mFFTLow.x = mVisualizer.getFFTLow();
    mFFTLow.y += mFFTLow.x * deltaTime;
    mFFTMid.x = mVisualizer.getFFTMid();
    mFFTMid.y += mFFTMid.x * deltaTime;
    mFFTHigh.x = mVisualizer.getFFTHigh();
    mFFTHigh.y += mFFTHigh.x * deltaTime;

    auto& cmds = mRenderer.Commands();
    cmds.Clear();

    cmds.Push<Commands::BindFramebuffer>(0);
    const glm::ivec2 size = mWindow.GetFramebufferSize();
    cmds.Push<Commands::Viewport>(0, 0, size.x, size.y);

    cmds.Push<Commands::ClearColor>(0.18f, 0.18f, 0.18f, 1.0f);
    cmds.Push<Commands::Clear>(GL_COLOR_BUFFER_BIT);

    if (!mCurrentProject.empty()) {
        cmds.Push<Commands::BindFramebuffer>(mRenderTarget->GetFramebuffer());
        cmds.Push<Commands::Viewport>(
            0, 0, mRenderTarget->GetSize().x, mRenderTarget->GetSize().y);

        cmds.Push<Commands::UseProgram>(mProgram->mProgram);
        cmds.Push<Commands::Uniform>(mFrameLoc, frame);
        cmds.Push<Commands::Uniform>(mTimeLoc, demoTime);
        cmds.Push<Commands::Uniform>(mResolutionLoc,
                                     glm::vec2(mRenderTarget->GetSize()));
        cmds.Push<Commands::Uniform>(mCameraOriginLoc, mCamera.GetPosition());
        cmds.Push<Commands::Uniform>(mCameraTargetLoc, mCamera.GetTarget());
        cmds.Push<Commands::Uniform>(mFFTLowLoc, mFFTLow);
        cmds.Push<Commands::Uniform>(mFFTMidLoc, mFFTMid);
        cmds.Push<Commands::Uniform>(mFFTHighLoc, mFFTHigh);
        mEditor.RegisterCommands(cmds, *mProgram);

        cmds.Push<Commands::BindVertexArray>(mVertexArray);
        cmds.Push<Commands::VertexAttribPointer>(mPosLoc,
                                                 2,
                                                 GL_FLOAT,
                                                 GL_FALSE,
                                                 (GLsizei)sizeof(vertices[0]),
                                                 nullptr);
        cmds.Push<Commands::EnableVertexAttribArray>(mPosLoc);
        cmds.Push<Commands::DrawArrays>(GL_TRIANGLES, 0, 3);
    }

    cmds.Push<Commands::BindFramebuffer>(0);

    mRenderer.ProcessCommands();

    mFileWatcher.Flush();
}

void Takoyaki::OnInput(const KeyInput& input) {
    mEditor.OnInput(input);
}

void Takoyaki::OnInput(const MouseInput& input) {
    mEditor.OnInput(input);
}

void Takoyaki::OnInput(const CursorInput& input) {
    mEditor.OnInput(input);
}

void Takoyaki::OnFramebufferSize(const glm::ivec2& size) {
    mEditor.OnFramebufferSize(size);
}

void Takoyaki::CreateVertexBuffer() {
    glGenBuffers(1, &mVertexArray);
    glBindBuffer(GL_ARRAY_BUFFER, mVertexArray);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glGenVertexArrays(1, &mVertexArray);
}

void Takoyaki::CreateRenderTarget() {
    mRenderTarget = std::make_unique<RenderTarget>(glm::ivec2{1280, 720});
}

void Takoyaki::SetupListeners() {
    mWindow.AddInputListener([this](const KeyInput& input) { OnInput(input); });
    mWindow.AddInputListener(
        [this](const MouseInput& input) { OnInput(input); });
    mWindow.AddInputListener(
        [this](const CursorInput& input) { OnInput(input); });
    mWindow.AddFramebufferSizeListener(
        [this](const glm::ivec2& size) { OnFramebufferSize(size); });

    mEditor.SetNewFileHandler([this]() { OnNewFile(); });
    mEditor.SetOpenFileHandler([this]() { OnOpenFile(); });
    mEditor.SetSaveFileHandler([this]() { OnSaveFile(); });

    mUniformsMenu.SetUniformsChangedHandler([this]() { OnUniformsChanged(); });
}

void Takoyaki::LoadShader() {
    mProgram = nullptr;

    std::ifstream shaderFile(mCurrentProject);
    std::string   shaderFileCode((std::istreambuf_iterator<char>(shaderFile)),
                               std::istreambuf_iterator<char>());
    shaderFile.close();
    std::string shaderCode = fragmentShaderCodeBegin;
    shaderCode += mUniformsMenu.GetUniformDeclarations();
    shaderCode += "#line 1\n" + shaderFileCode;
    shaderCode += fragmentShaderCodeEnd;

    VertexShader   vShader(vertexShaderCode);
    FragmentShader fShader(shaderCode.c_str());

    mProgram = std::make_unique<ShaderProgram>(vShader, fShader);

    auto error = mProgram->GetError();
    if (error.has_value()) {
        mEditor.ReportError(error.value());
    } else {
        mEditor.ReportError("Successfully Compiled Shader! :D");
    }

    mPosLoc          = mProgram->GetAttributeLocation("vPos");
    mFrameLoc        = mProgram->GetUniformLocation("iFrame");
    mTimeLoc         = mProgram->GetUniformLocation("iTime");
    mResolutionLoc   = mProgram->GetUniformLocation("iResolution");
    mCameraOriginLoc = mProgram->GetUniformLocation("iCameraOrigin");
    mCameraTargetLoc = mProgram->GetUniformLocation("iCameraTarget");
    mFFTLowLoc       = mProgram->GetUniformLocation("iFFTLow");
    mFFTMidLoc       = mProgram->GetUniformLocation("iFFTMid");
    mFFTHighLoc      = mProgram->GetUniformLocation("iFFTHigh");
}

void Takoyaki::OnNewFile() {
    const char* fileToCreate = tinyfd_saveFileDialog(
        "Open TakoYaki file", "", 1, &glslFileTypeFilter, nullptr);
    if (fileToCreate) {
        std::ofstream f(fileToCreate, std::ios_base::trunc);
        if (!f.is_open()) {
            mEditor.ReportError("Failed to create new file!");
            return;
        }

        f << templateShaderCode;
        f.close();
        LoadProjectFile(fileToCreate);
    }
}

void Takoyaki::OnOpenFile() {
    const char* fileToLoad = tinyfd_openFileDialog(
        "Open TakoYaki file", "", 1, &glslFileTypeFilter, nullptr, 0);
    if (fileToLoad) {
        LoadProjectFile(fileToLoad);
    }
}

void Takoyaki::OnSaveFile() {
    if (!mCurrentProject.empty()) {
        mUniformsMenu.SaveFile(mCurrentProject);
    }
}

void Takoyaki::LoadProjectFile(const char* fileToLoad) {
    mCurrentTime = 0.0f;
    mFFTLow = mFFTMid = mFFTHigh = {0, 0};

    mCurrentProject = fileToLoad;

    mEditor.LoadProjectFile(mCurrentProject);

    LoadShader();
    mFileWatcher.Clear();
    mFileWatcher.Watch(mCurrentProject, [this](auto&) { LoadShader(); });
}

void Takoyaki::OnUniformsChanged() {
    LoadShader();
}

}  // namespace ht