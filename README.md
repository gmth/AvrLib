BUGS
====
 - Make rt->counts return Counts<> instead of uint32_t
 - log::debug should always invoke toString(T), which defaults to dec() for decimals,
   but also defines Microsecond<> as "...ms", etc.
 - Make RFM12 OOK support optional, so you can create the RFM12 driver without 
   using a comparator with suitable timer prescaler
 - Scan for chunked-fifo like API, drop whole chunk if not matched. 
      Or, if scan can work within a chunk, make up a different name, e.g. match()
      
 - Create a TypedFifo, templated on sizeof(T), without the generic write and read methods; 
    * only one T at a time
    * maybe on(T, lambda)
 - find out why pulseCounter.minimumLength is somehow applied x2
 - Have F("12") for length <= 2 (or 3) just be plain C strings, since they use as much mem but much faster.   
-  Add JSON (or protobuf?) types for easier packet output.
     
 - Rewrite SerialConfig to be a static template class, and remove (for now) ability to change serial configs at
   runtime. That'll create much faster software serial, and removes the need to juggle pointers in the fifo.
 - investigate replacing INTERRUPT_HANDLER1, 2, etc. macros with a macro-block
     INTERRUPT_HANDLERS {   // starts a nested type with a single, known name
       INTERRUPT_HANDLER(vect, method);  // declares a single method with a known name, vect is template param for the method.. but what about multi?
     };
 - use the above for an easy interrupt forwarding macro
  
 - maintain "last sent" pin value in pulse counter, comparing with pin->isHigh() after each change, inserting dummies on mismatch
   . That way we don't need the double memory anymore after all.
 - consider pin factory methods to imply default state as input / output
 - make namespace HAL::Atmel consistent (everything public goes there, everything private in sub-namespaces)

- Change fifo.abortedWrites into a flag "overflow", and add two other flags "full" and "hasData", so
  fastread() and fastwrite() don't have to invoke complex calculations.
- create template variants of the PinChange handlers that don't allow runtime setting of onRising / onFalling
  but rather do it in template, to save cycles.
  
PROTOCOL
========
"Status update" protocol

   - Only send current state from A to B
      - Don't enqueue if the same as last enqueued / transmitted
      - Only buffer 1 packet, plus 1 packet being sent
   - Only receive desired state from B to A
      - Only keep last received, plus currently being received
      