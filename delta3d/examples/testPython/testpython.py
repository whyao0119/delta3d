from dtCore import *
from dtABC import *

from math import *
from time import *

def radians(v):
   return v * pi/180.0
   
class TestPythonApplication(Application):
    def Config(self):
        Application.Config(self)
        SetDataFilePathList('../../data')
        self.helo = Object('UH-1N')
        self.helo.LoadFile('models/uh-1n.ive')
        self.AddDrawable(self.helo)
        self.transform = Transform()
        self.angle = 0.0
        
    def PreFrame(self, deltaFrameTime):
        translation = Vec3(40.0*cos(radians(self.angle)),
                           100.0 + 40.0*sin(radians(self.angle)), 
                           0.0)
        rotation = Vec3(self.angle, 0.0, -45.0)
        self.transform.SetTranslation(translation)
        self.transform.SetRotation(rotation)
        self.helo.SetTransform(self.transform)
        self.angle += 45.0*deltaFrameTime

testPythonApp = TestPythonApplication('config.xml')

testPythonApp.Config()
testPythonApp.Run()
