import SwiftUI

struct PaintView: View {
    @State private var currentColor = Color.black
    @State private var lineWidth: CGFloat = 5.0
    @State private var drawings = [[Drawing]]()
    @State private var currentDrawing = [Drawing]()
    @State private var cleared = false
    
    var body: some View {
        VStack {
            Spacer()
            
            ZStack {
                Rectangle()
                    .fill(Color.white)
                    .gesture(DragGesture(minimumDistance: 0)
                        .onChanged({ value in
                            let point = value.location
                            if cleared {
                                cleared = false
                                drawings = []
                                currentDrawing = []
                            }
                            let previousPoint = currentDrawing.last?.point ?? point
                            currentDrawing.append(Drawing(point: point, previousPoint: previousPoint, color: currentColor, lineWidth: lineWidth))
                        })
                        .onEnded({ value in
                            drawings.append(currentDrawing)
                            currentDrawing = []
                        })
                    )

                    .drawingGroup()
                
                ForEach(drawings, id: \.self) { drawing in
                    Path { path in
                        for (i, element) in drawing.enumerated() {
                            if i == 0 {
                                path.move(to: element.point)
                            } else {
                                path.addLine(to: element.point)
                            }
                        }
                    }
                    .stroke(drawing.first?.color ?? currentColor, lineWidth: drawing.first?.lineWidth ?? lineWidth)
                    .animation(.linear)
                }
                
                Path { path in
                    for (i, element) in currentDrawing.enumerated() {
                        if i == 0 {
                            path.move(to: element.point)
                        } else {
                            path.addLine(to: element.point)
                        }
                    }
                }
                .stroke(currentColor, lineWidth: lineWidth)
                .animation(nil)
            }
            .frame(maxWidth: .infinity, maxHeight: .infinity)
            .background(Color.white)
            
            Spacer()
            
            HStack {
                Button(action: {
                    cleared = true
                    drawings = []
                    currentDrawing = []
                }, label: {
                    Image(systemName: "trash")
                })
                
                Spacer()
                
                ColorPicker("Color", selection: $currentColor)
                
                Spacer()
                
                Slider(value: $lineWidth, in: 1...20, step: 1)
                    .frame(width: 150)
            }
            .padding()
        }

    }
    
}

struct Drawing: Hashable {
    let id = UUID()
    let point: CGPoint
    let previousPoint: CGPoint?
    let color: Color
    let lineWidth: CGFloat
    
    func hash(into hasher: inout Hasher) {
        hasher.combine(id)
    }
    
    static func == (lhs: Drawing, rhs: Drawing) -> Bool {
        lhs.id == rhs.id
    }
    
    init(point: CGPoint, previousPoint: CGPoint? = nil, color: Color, lineWidth: CGFloat) {
        self.point = point
        self.previousPoint = previousPoint
        self.color = color
        self.lineWidth = lineWidth
    }
}


struct PaintView_Previews: PreviewProvider {
    static var previews: some View {
        PaintView()
    }
}
