//
//  ContentView.swift
//  Auto_stove
//
//  Created by Hien Le on 5/11/21.
//

import SwiftUI
import UserNotifications

struct stoveState: Identifiable {
    
    var id: AlertType
    
    enum AlertType {
	   case stoveOn
	   case stoveOff
    }
}

struct commandState: Identifiable {
    
    var id: commandType
    
    enum commandType {
	   case turningOff
	   case alreadyOff
    }
}

struct ContentView: View {
    @State var stove: stoveState?
    @State var command: commandState?
    var body: some View {
	ZStack {
	    Color.black
		   .ignoresSafeArea()

	    VStack{
		   
		   Button("Turn Off Stove") {
		    checkStove(completion: { response in
			   print(response)
			   if response == "0"{
				 self.command = commandState(id: .alreadyOff)
			   }
			   if response == "1"{
				 self.command = commandState(id: .turningOff)
		    }})}.alert(item: $command) { (command) -> Alert in
			    switch command.id {
				case .turningOff:
				    turnOffStove(completion: { response in
				    print("turning Off Stove")})
				    return Alert(title: Text("Turning Your Stove Off"))
				case .alreadyOff:
				    return Alert(title: Text("Your Stove is Off"))
				}
		    }
			.foregroundColor(.purple)
		    .font(.title).padding()
		    .border(Color.purple, width: 5).padding(.bottom, 200)
		    
		    
		   Button("Check Stove"){
			  checkStove(completion: { response in
				 print(response)
				 if response == "0"{
				    self.stove = stoveState(id: .stoveOff)
				 }
				 if response == "1"{
				    self.stove = stoveState(id: .stoveOn)
				 }})
		   }.alert(item: $stove) { (stove) -> Alert in
			  switch stove.id {
				  case .stoveOn:
					 return Alert(title: Text("Your Stove is On"))
				  case .stoveOff:
					 return Alert(title: Text("Your Stove is Off"))
				  }}.foregroundColor(.purple)
		   .font(.title).padding()
		   .border(Color.purple, width: 5)
		    
	    }
	}
}
	func turnOffStove(completion: @escaping (String) -> Void) {
		let url = URL(string: "https://auto-stove.herokuapp.com/turnOff/?command=turnOff")!

		let task = URLSession.shared.dataTask(with: url)
		{(data, response, error) in
		    guard let data = data else { return }
		    guard response != nil else { return }
		    completion(String(data: data, encoding: .utf8)!)
		}

		task.resume()
		
	}

}

func checkStove(completion: @escaping (String) -> Void) {
	let url = URL(string: "https://auto-stove.herokuapp.com/getState/")!

	let task = URLSession.shared.dataTask(with: url)
	{(data, response, error) in
	    guard let data = data else { return }
	    guard response != nil else { return }
	    completion(String(data: data, encoding: .utf8)!)
	}

	task.resume()
	
}

struct ContentView_Previews: PreviewProvider {
    static var previews: some View {
        ContentView()
    }
}
