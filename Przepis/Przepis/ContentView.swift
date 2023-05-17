import SwiftUI
import Foundation
import WebKit

struct Meal: Decodable, Identifiable {
let id: Int
let title: String
let image: String
let instructions: String?
}

//struct RemoteImage: View {
//let url: String
//let placeholderImage = UIImage(named: "placeholder")
//    var body: some View {
//        AsyncImage(url: URL(string: url), scale: 1.0) { image in
//            image
//                .resizable()
//                .aspectRatio(contentMode: .fill)
//        } placeholder: {
//            Image(uiImage: placeholderImage ?? UIImage())
//                .resizable()
//                .aspectRatio(contentMode: .fill)
//        }
//        .frame(height: 150)
//    }
//}
struct RemoteImage: View {
    let url: String
    let placeholderImage = UIImage(named: "placeholder")
    var body: some View {
        AsyncImage(url: URL(string: url), scale: 1.0) { image in
            image
                .resizable()
                .scaledToFit()
        } placeholder: {
            Image(uiImage: placeholderImage ?? UIImage())
                .resizable()
                .scaledToFit()
        }
        .padding(.leading, 10)
        .padding(.trailing, 10)
//        .frame(width: 150, height: 150)
    }
}

struct InstructionsView: View {
    let meal: Meal
    @Binding var showInstructions: Bool
    @State private var webViewHeight: CGFloat = .zero
    @State private var instructions: String?

    var body: some View {
        VStack {
            RemoteImage(url: meal.image)
                .aspectRatio(contentMode: .fit)
                .padding()

            Text(meal.title)
                .font(.title)

            Section(header: Text("Instructions:")) {
                if let instructions = instructions {
                    ScrollView {
                        Text(instructions)
                            .padding()
                    }
                } else {
                    ProgressView()
                        .onAppear {
                            fetchInstructions()
                        }
                }
            }

            Button(action: {
                showInstructions = false
            }) {
                Text("Close")
                    .foregroundColor(.white)
                    .padding(.vertical, 10)
                    .padding(.horizontal, 20)
                    .background(Color.brown)
                    .cornerRadius(10)
            }
            .padding(.top, 20)
            
        }
    }

    func fetchInstructions() {
        let apiKey = "fdc842a2986742deb7ac3793ce07b8d5"
        let url = "https://api.spoonacular.com/recipes/\(meal.id)/analyzedInstructions?apiKey=\(apiKey)"

        guard let urlObj = URL(string: url) else {
            print("Invalid URL")
            return
        }

        URLSession.shared.dataTask(with: urlObj) { data, response, error in
            guard let data = data else {
                print("Error: \(error?.localizedDescription ?? "Unknown error")")
                return
            }

            do {
                let instructionsData = try JSONDecoder().decode([Instruction].self, from: data)
                let instructions = instructionsData.compactMap { $0.steps }.joined().compactMap { $0.step }
                DispatchQueue.main.async {
                    self.instructions = instructions.joined(separator: "\n\n")
                }
            } catch let error {
                print("Error decoding instructions: \(error.localizedDescription)")
            }
        }.resume()
    }
}

struct Instruction: Decodable {
    let steps: [Step]?

    struct Step: Decodable {
        let step: String?
    }
}

struct WebView: UIViewRepresentable {
let url: String
@Binding var contentHeight: CGFloat
    func makeUIView(context: Context) -> WKWebView {
        let webView = WKWebView()
        webView.navigationDelegate = context.coordinator
        webView.load(URLRequest(url: URL(string: url)!))
        return webView
    }

    func updateUIView(_ uiView: WKWebView, context: Context) {}

    func makeCoordinator() -> Coordinator {
        Coordinator(self)
    }

    class Coordinator: NSObject, WKNavigationDelegate {
        let webView: WebView

        init(_ webView: WebView) {
            self.webView = webView
        }

        func webView(_ webView: WKWebView, didFinish navigation: WKNavigation!) {
            webView.evaluateJavaScript("document.readyState", completionHandler: { (complete, error) in
                if complete != nil {
                    webView.evaluateJavaScript("document.body.scrollHeight", completionHandler: { (height, error) in
                        if let height = height as? CGFloat {
                            webView.scrollView.isScrollEnabled = false
                            self.webView.contentHeight = height
                        }
                    })
                }
            })
        }
    }
}

struct MealsResponse: Decodable {
let results: [Meal]
}

struct ContentView: View {
    @State private var mealType = "Lunch"
    @State private var ingredients = ""

    @State private var meals: [Meal] = []
    @State private var isLoading = false

    @State private var showInstructions = false
    @State private var selectedMeal: Meal?

    var body: some View {
        
        VStack {
            HStack {
                Text("Meal Type:").padding(.leading, 10).bold()
                Picker(selection: $mealType, label: Text("")) {
                    Text("Breakfast").tag("Breakfast").bold()
                    Text("Lunch").tag("Lunch").bold()
                    Text("Dinner").tag("Dinner").bold()
                }
                .pickerStyle(SegmentedPickerStyle())
                
            }

            TextField("Enter ingredients, separated by comma", text: $ingredients)
                .textFieldStyle(RoundedBorderTextFieldStyle())
                .padding(.horizontal)

            Button(action: {
                self.fetchMeals()
            }) {
                Text("Search")
                    .foregroundColor(.white)
                    .padding(.vertical, 10)
                    .padding(.horizontal, 20)
                    .background(Color.brown)
                    .cornerRadius(10)
            }
            .padding(.top, 20)

            if isLoading {
                ProgressView()
            } else if !meals.isEmpty {
                ScrollView {
                    LazyVGrid(columns: [GridItem(.adaptive(minimum: 150))]) {
                        ForEach(meals) { meal in
                            VStack(alignment: .leading) {
                                RemoteImage(url: meal.image)
                                    .aspectRatio(contentMode: .fit)
                                    .onTapGesture {
                                        selectedMeal = meal
                                        showInstructions = true
                                    }

                                Button(action:{
                                    selectedMeal = meal
                                    showInstructions = true
                                }) {
                                    Text("Show Instructions")
                                        .foregroundColor(.white)
                                        .padding(.vertical, 10)
                                        .padding(.horizontal, 10)
                                        .background(Color.brown)
                                        .cornerRadius(10)
                                }
                                .padding(.leading, 10)
                                Text(meal.title)
                                    .font(.system(size: 13, weight: .bold))

                                    //.font(.headline)
                                    .padding(.leading, 20)
                                    
                            }
                        }
                    }
                    .navigationTitle("Recipe Search")
                    .sheet(isPresented: $showInstructions) {
                        if let selectedMeal = selectedMeal {
                            InstructionsView(meal: selectedMeal, showInstructions: $showInstructions)
                        }
                    }
                }
            } else {
                Text("No results found.")
            }
        }
        .background(Image("tlo")
            .resizable()
            .scaledToFill()
            .frame(width: UIScreen.main.bounds.width, height: UIScreen.main.bounds.height)
            .edgesIgnoringSafeArea(.all)
            .opacity(0.5))
    }

    func fetchMeals() {
        isLoading = true

        let query = ingredients.addingPercentEncoding(withAllowedCharacters: .urlHostAllowed) ?? ""
        let apiKey = "fdc842a2986742deb7ac3793ce07b8d5" // replace with your own API key
        let endpoint = "https://api.spoonacular.com/recipes/complexSearch?apiKey=\(apiKey)&query=\(query)&type=\(mealType)"
        guard let url = URL(string: endpoint) else {
            print("Invalid URL")
            return
        }

//        let url = URL(string: "https://api.spoonacular.com/recipes/complexSearch?apiKey=\(apiKey)&addRecipeInformation=true&number=10&query=\(query)&mealType=\(mealType)")!

        URLSession.shared.dataTask(with: url) { data, response, error in
            defer {
                isLoading = false
            }

            if let data = data {
                if let decodedResponse = try? JSONDecoder().decode(MealsResponse.self, from: data) {
                    DispatchQueue.main.async {
                        self.meals = decodedResponse.results
                    }

                    return
                }
            }

            print("Fetch failed: \(error?.localizedDescription ?? "Unknown error")")
        }.resume()
    }
}

struct ContentView_Previews: PreviewProvider {
static var previews: some View {
ContentView()
}
}

struct InstructionsView_Previews: PreviewProvider {
static var previews: some View {
InstructionsView(meal: Meal(id: 1, title: "Spaghetti and Meatballs", image: "https://example.com/spaghetti.jpg", instructions: "1. Cook spaghetti according to package instructions. 2. In a large bowl, mix together ground beef, breadcrumbs, egg, garlic, salt, and pepper. 3. Form mixture into meatballs. 4. Heat oil in a large skillet over medium heat. 5. Add meatballs and cook until browned on all sides. 6. Add tomato sauce and bring to a simmer. 7. Cover and simmer for 15 minutes. 8. Serve meatballs and sauce over spaghetti."), showInstructions: .constant(true))
}
}
