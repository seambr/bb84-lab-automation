# BB84 Protocol

## What is it?

The BB84 protocol utilizes the uncertainty principle of quantum mechanics and a set of conjugate polarization states of light in order to generate a one time secret key, also known as a `one time pad`, that can be used to communicate securely. The destructive nature of quantum measurements ensures that anyone trying to eavesdrop on this key generation will have a high probability of disrupting the key generation process and thus be detected.

## Why Does It Work?

This readme will focus mainly on the implementation of an automated version of the protocol. For further information see [this poster](docs/BB84_Quantum_Protocol_Adem_Akyasar_Poster.pdf), [this writeup](docs/Quantum_Cryptography_BB84.pdf), or [Bennett and Brassard's original 1984 paper](https://doi.org/10.1016/j.tcs.2014.05.025).

<img alt="theory_image" src="images/snips/theory_snip.png" class="theory-img"></img>

<style>
  .theory-img{
    max-width: 200px;
  }
</style>

## The Protocol[^1]

A sender, "Alice", first selects a random sequence of bits as well as bases. "Alice" then goes through these sequences and sends photons encoded in the polarization state corresponding to the random basis and bit. With a $0^\circ$ and $-45^\circ$ polarized photon representing a **logical 0** in the rectilinear and diagonal bases respectively; while a $90^\circ$ and $45^\circ$ polarized photon represents a **logical 1** in the rectilinear and diagonal bases respectively.

A receiver, "Bob", independently selects a random sequence of bases to measure in. "Bob" measures each photon "Alice" sends and interprets each measurement as a **logical 1** or **logical 0**. If "Bob" did not measure in the same basis that "Alice" sent in then the interpreted bit is entirely random.

Now, both parties have a sequence of bits along with a sequence of bases in which those bits were measured or sent. "Alice" and "Bob" can now publicly compare their base sequences, they agree to disregard any bits in their sequence where thecorresponding basis do not agree. They can also determine if any photons were lost in transit and discard those. If allwent well "Alice" and "Bob" should each have a matching key known only to them.

---

## The Task At Hand

We need a way to perform this protocol automatically. We can split this into three parts:

### Alice(The Sender)

1. Toggle a linearly polarized laser on and off with microsecond precision to generate pulses
2. Rotating the polarization of our light pulse to our desired bit and basis automatically
3. Keeping track of outgoing states

### Bob(The Receiver)

1. Measuring the polarization of an incoming pulse
2. Keeping track of incoming states
3. Change Bases automatically

### Eve(The Eavesdropper)

Eve
