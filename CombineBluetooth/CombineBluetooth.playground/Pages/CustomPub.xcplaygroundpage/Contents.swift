//: [Previous](@previous)

import Foundation
import Combine

enum Access {
    case readonly
    case readwrite
    case writeonly
}

enum Errrrrr: Error {
    case fish
}

protocol DataConv{
    
}

extension Int: DataConv{}


@propertyWrapper
class Characteristic<Value: DataConv>{

    
    var wrappedValue: Value?
    init(wrappedValue value: Value? = nil){
        wrappedValue = value
    }
}

class Testeron{
    @Characteristic var one: Int?
    init(){}
}

let testeron = Testeron()
testeron.one
testeron.one = 500
testeron.one




//
//@propertyWrapper
//class Characteristic<Value>: Publisher {
//
//    typealias Output = Value?
//    typealias Failure = Error
//
//    func receive<S>(subscriber: S) where S : Subscriber, Characteristic.Failure == S.Failure, Characteristic.Output == S.Input {
//        subject.receive(subscriber: subscriber)
//    }
//
//    var _value: Value? {
//        didSet{
//            subject.send(_value)
//        }
//    }
//
//    var _access: Access
//
//    let subject = PassthroughSubject<Value?,Error>()
//
//    var wrappedValue: Value? {
//        get {
//            switch _access{
//            case .readonly, .readwrite:
//                return _value
//            case .writeonly:
//                return nil
//            }
//
//        }
//        set {
//            switch _access {
//            case .readonly:
//                break
//            case .readwrite, .writeonly:
//                _value = newValue
//            }
//        }
//    }
//
//    init(wrappedValue value: Value? = nil, _ uuid: UUID, access: Access){
//        _value = value
//        _access = access
//    }
//
//    func update(newValue: Value){
//        _value = newValue
//    }
//
//    var projectedValue: Characteristic{
//        return self
//    }
//    func err(){
//        subject.send(completion: .failure(Errrrrr.fish))
//    }
//}
//
//class Thingy {
//    @Characteristic(UUID(), access: .readonly) var foo: Int
//    @Characteristic(UUID(), access: .readwrite) var bar: Int
//    @Characteristic(UUID(), access: .writeonly) var baz: Int
//}
//
//
//let thingy = Thingy()
//
//let c = thingy.$foo.sink(receiveCompletion: { (completion) in
//    print("c completion", completion)
//}) { (new) in
//    print("c new", new ?? "nil")
//}
//
//
//
//thingy.foo
//thingy.foo = 500
//thingy.foo
//
//thing.bar = 120
//
//thingy.$foo.update(newValue: 200)
//thingy.foo
//
//let d = thingy.$foo.sink(receiveCompletion: { (completion) in
//    print("d completion", completion)
//}) { (new) in
//    print("d new", new ?? "nil")
//}
//
//
//thingy.$foo.update(newValue: 300)
//thingy.$foo.update(newValue: 400)
//thingy.$foo.err()
//
////thingy.whatnot = 6
//
//
//
////: [Next](@next)
