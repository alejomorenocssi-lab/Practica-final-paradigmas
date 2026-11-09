/* =========================================================================
  DAVID ALEJANDRO CASTAÑO MORENO
  BRAYAN GRISALES PEREZ
   =========================================================================
*/

/*Bibliotecas (no librerias explicado por dieguito)*/
#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <algorithm>
#include <numeric>
#include <map>
#include <functional>

/* -------------------------------------------------------------------------
   Utility: Colombian peso formatting (3820 -> "$ 3.820")
   entregada por el profesor NO MODIFICAR
   ------------------------------------------------------------------------- */
std::string formatCOP(long long value) {
    std::string digits = std::to_string(value);
    std::string out;
    int count = 0;
    for (auto it = digits.rbegin(); it != digits.rend(); ++it) {
        out.push_back(*it);
        if (++count % 3 == 0 && it + 1 != digits.rend()) {
            out.push_back('.');
        }
    }
    std::reverse(out.begin(), out.end());
    return "$ " + out;
}

/* -------------------------------------------------------------------------
   PART 1. Abstract base class of the transport-mode hierarchy
   ------------------------------------------------------------------------- */
class TransportMode {
    //En c++ lo que define si es abstractra es la presencia de funciones virtuales puras
    
    //Constructor (Código otorgado no mover)
public:
    TransportMode(std::string name, int stations)
        : name_(std::move(name)), stations_(stations) {}

    virtual ~TransportMode() = default;          // mandatory: deleted through base pointer
    
    //getters de name y stations
    const std::string& name() const { return name_; }
    int stations() const { return stations_; }
    
    

    // 1.1 Pure virtual: the base class cannot know either value. son 
    virtual long long baseFare() const = 0;
    virtual std::string serviceType() const = 0;

    // 1.2 Policy hooks, default true. Overridden only where behaviour differs.
    virtual bool allowsStudentFlatFare() const { return true; }
    virtual bool allowsIntegration() const { return true; }
    virtual bool opensIntegrationWindow() const { return true; }

    // 1.3 Non-virtual template method: the algorithm is written once here
    //     and calls the pure virtual members through this object.
    std::string describe() const {
        return serviceType() + " " + name_ + " (" + std::to_string(stations_) +
               " stations) - full fare " + formatCOP(baseFare());
    }

private:
    std::string name_;
    int stations_;
};

// 1.4 Concrete modes
class MetroLine : public TransportMode {
public:
    MetroLine(std::string name, int stations)
        : TransportMode(std::move(name), stations) {}

    long long baseFare() const override { return 3820; }
    std::string serviceType() const override { return "Metro line"; }
};

class TramLine : public TransportMode {
public:
    TramLine(std::string name, int stations)
        : TransportMode(std::move(name), stations) {}

    long long baseFare() const override { return 3820; }
    std::string serviceType() const override { return "Tram line"; }
};

class CableLine : public TransportMode {
public:
    CableLine(std::string name, int stations)
        : TransportMode(std::move(name), stations) {}

    long long baseFare() const override { return 3820; }
    std::string serviceType() const override { return "Cable line"; }
};

//Las funciones de la clase basse (ganchos) se sobreescriben en estas dos últimas clases
class TouristCable : public CableLine {
public:
    TouristCable(std::string name, int stations, long long surcharge)
        : CableLine(std::move(name), stations), surcharge_(surcharge) {}

    // Parent fare through a qualified call: the literal is not repeated here.
    long long baseFare() const override { return CableLine::baseFare() + surcharge_; }
    std::string serviceType() const override { return "Tourist cable"; }

    bool allowsStudentFlatFare() const override { return false; }
    bool allowsIntegration() const override { return false; }
    bool opensIntegrationWindow() const override { return false; }

private:
    long long surcharge_;
};

class FeederRoute : public TransportMode {
public:
    FeederRoute(std::string name, int stations)
        : TransportMode(std::move(name), stations) {}

    long long baseFare() const override { return 0; }
    std::string serviceType() const override { return "Feeder route"; }

    // Free because the trunk fare covers it: it must not open a window.
    bool opensIntegrationWindow() const override { return false; }
};

/* -------------------------------------------------------------------------
   PART 2. Passenger hierarchy, Non-Virtual Interface idiom
   fareOn() is NOT virtual: it is the template method that clamps the result.
   computeFare() is the customisation point each profile overrides.
   ------------------------------------------------------------------------- */
class Passenger {
public:
    //Se usa explicit porque solo tiene un parametro y por buena practica (como es abstracta no es tan necesario )
    explicit Passenger(std::string name = "") : name_(std::move(name)) {}
    
    //Destructor
    virtual ~Passenger() = default;
    
    //metodo virtual puro
    virtual std::string profile() const = 0;

    // NOT virtual: calls computeFare and clamps, a fare is never negative.
    //Recuerda que el ampersan significa una referencia
    //Recibe un objeto de clase transporte y lo nombra mode, evita que sea negativo
    long long fareOn(const TransportMode& mode) const {
        return std::max(0LL, computeFare(mode));
    }
    
    //getter
    const std::string& name() const { return name_; }

protected:
    // Default behaviour: the full fare of the mode.
    virtual long long computeFare(const TransportMode& mode) const {
        return mode.baseFare();
    }

private:
    std::string name_;
};

class FrequentPassenger : public Passenger {
public:
    explicit FrequentPassenger(std::string name = "") : Passenger(std::move(name)) {}
    std::string profile() const override { return "Civica frequent"; }
    // Overrides nothing: it inherits the default full fare.
};

class StudentPassenger : public Passenger {
public:
    explicit StudentPassenger(std::string name = "") : Passenger(std::move(name)) {}
    std::string profile() const override { return "Municipal student"; }

protected:
    long long computeFare(const TransportMode& mode) const override {
        // Asks the mode for its policy, never for its class.
        if (mode.allowsStudentFlatFare()) {
            return 1600;
        }
        return Passenger::computeFare(mode);
    }
};

class SeniorPassenger : public Passenger {
public:
    explicit SeniorPassenger(std::string name = "") : Passenger(std::move(name)) {}
    std::string profile() const override { return "Senior citizen"; }

protected:
    long long computeFare(const TransportMode& mode) const override {
        return Passenger::computeFare(mode) - 490;
    }
};

class DisabilityPassenger : public Passenger {
public:
    explicit DisabilityPassenger(std::string name = "") : Passenger(std::move(name)) {}
    std::string profile() const override { return "Person with disability"; }

protected:
    long long computeFare(const TransportMode& mode) const override {
        return Passenger::computeFare(mode) - 1100;
    }
};

class BearerPassenger : public Passenger {
public:
    explicit BearerPassenger(std::string name = "") : Passenger(std::move(name)) {}
    std::string profile() const override { return "Bearer (occasional)"; }

protected:
    long long computeFare(const TransportMode& mode) const override {
        return Passenger::computeFare(mode) + 580;
    }
};

/* -------------------------------------------------------------------------
   PART 3. The itinerary and the integration rule
   ------------------------------------------------------------------------- */
struct Leg {
    int minute;                   // minutes elapsed since the journey started
    const TransportMode* mode;    // non-owning observer pointer
};

struct ChargedLeg {
    int minute;
    std::string modeName;
    std::string serviceType;
    long long charge;
    bool integrated;              // true: free transfer inside the window
};

// 3.1 Network: it OWNS its modes; main() only observes them.
class Network {
    std::vector<std::unique_ptr<TransportMode>> modes_;

public:
    TransportMode& add(std::unique_ptr<TransportMode> mode) {
        modes_.push_back(std::move(mode));
        return *modes_.back();
    }

    const std::vector<std::unique_ptr<TransportMode>>& modes() const {
        return modes_;
    }

    void printInventory() const {
        std::for_each(modes_.begin(), modes_.end(),
            [](const std::unique_ptr<TransportMode>& m) {
                std::cout << " - " << m->describe() << '\n';
            });
    }
};

// 3.2 Integration rule
std::vector<ChargedLeg> chargeItinerary(std::vector<Leg> legs,
                                        const Passenger& passenger) {
    constexpr int kWindowMinutes = 90;
    constexpr int kMaxFreeTransfers = 2;

    // a) sort by minute with std::sort and a lambda comparator
    std::sort(legs.begin(), legs.end(),
              [](const Leg& a, const Leg& b) { return a.minute < b.minute; });

    std::vector<ChargedLeg> charged;
    bool windowOpen = false;
    int windowStart = 0;
    int transfersUsed = 0;

    // The ONLY explicit loop in the whole program: it is the algorithm itself.
    for (const Leg& leg : legs) {
        const TransportMode& mode = *leg.mode;
        long long charge = 0;
        bool integrated = false;

        // d) and e): a mode that does not integrate, or does not open a window,
        //    pays its own fare and leaves the window untouched.
        const bool takesPartInWindow =
            mode.allowsIntegration() && mode.opensIntegrationWindow();

        if (!takesPartInWindow) {
            charge = passenger.fareOn(mode);
        } else if (windowOpen &&
                   leg.minute - windowStart <= kWindowMinutes &&
                   transfersUsed < kMaxFreeTransfers) {
            // c) free transfer inside the window
            integrated = true;
            ++transfersUsed;
        } else {
            // b) and f): pay the full fare and open a new window
            charge = passenger.fareOn(mode);
            windowOpen = true;
            windowStart = leg.minute;
            transfersUsed = 0;
        }

        charged.push_back({leg.minute, mode.name(), mode.serviceType(), charge, integrated});
    }
    return charged;
}

/* -------------------------------------------------------------------------
   PART 4. Reports built exclusively with STL algorithms and lambdas
   ------------------------------------------------------------------------- */
// 4.1
long long totalPaid(const std::vector<ChargedLeg>& legs) {
    return std::accumulate(legs.begin(), legs.end(), 0LL,
        [](long long sum, const ChargedLeg& l) { return sum + l.charge; });
}

// 4.2
long long freeTransferCount(const std::vector<ChargedLeg>& legs) {
    return std::count_if(legs.begin(), legs.end(),
        [](const ChargedLeg& l) { return l.integrated; });
}

// 4.3
std::map<std::string, long long> paidByServiceType(const std::vector<ChargedLeg>& legs) {
    std::map<std::string, long long> result;
    std::for_each(legs.begin(), legs.end(),
        [&result](const ChargedLeg& l) { result[l.serviceType] += l.charge; });
    return result;
}

// 4.4 The network does not know the criterion: the caller injects it.
std::vector<std::string> namesMatching(
    const Network& network,
    const std::function<bool(const TransportMode&)>& policy) {
    std::vector<std::string> result;
    const auto& modes = network.modes();
    std::for_each(modes.begin(), modes.end(),
        [&result, &policy](const std::unique_ptr<TransportMode>& m) {
            if (policy(*m)) {
                result.push_back(m->name());
            }
        });
    return result;
}

// 4.5
std::string joined(const std::vector<std::string>& names) {
    if (names.empty()) return "";
    return std::accumulate(std::next(names.begin()), names.end(), names.front(),
        [](const std::string& acc, const std::string& n) { return acc + ", " + n; });
}

/* -------------------------------------------------------------------------
   MAIN
   ------------------------------------------------------------------------- */
int main() {
    std::cout << "============================================================\n";
    std::cout << "   INTEGRATED ITINERARY AND FARE ENGINE - Valle de Aburra\n";
    std::cout << "============================================================\n\n";

    Network network;
    TransportMode& lineA  = network.add(std::make_unique<MetroLine>("A", 21));
    TransportMode& lineB  = network.add(std::make_unique<MetroLine>("B", 7));
    TransportMode& tramTA = network.add(std::make_unique<TramLine>("T-A Ayacucho", 9));
    TransportMode& cableK = network.add(std::make_unique<CableLine>("K Santo Domingo", 4));
    TransportMode& arvi   = network.add(std::make_unique<TouristCable>("L Arvi", 2, 9180));
    TransportMode& feeder = network.add(std::make_unique<FeederRoute>("130 Robledo", 18));

    std::cout << ">> 1. NETWORK INVENTORY (polymorphic describe)\n";
    network.printInventory();
    std::cout << '\n';

    /* Itinerary deliberately given out of chronological order, so that the
       sort step is observable. */
    const std::vector<Leg> itinerary = {
        {45,  &cableK},
        {0,   &feeder},
        {12,  &lineA},
        {150, &lineB},
        {70,  &tramTA},
        {200, &arvi}
    };

    const FrequentPassenger frequent("Diego");
    const StudentPassenger  student("Valentina");
    const SeniorPassenger   senior("Dona Amparo");

    std::cout << ">> 2. FULL FARE PER PROFILE ON METRO LINE A\n";
    std::cout << "  " << frequent.profile() << " pays " << formatCOP(frequent.fareOn(lineA)) << '\n';
    std::cout << "  " << student.profile()  << " pays " << formatCOP(student.fareOn(lineA))  << '\n';
    std::cout << "  " << senior.profile()   << " pays " << formatCOP(senior.fareOn(lineA))   << '\n';
    std::cout << "  " << student.profile()  << " on the tourist cable pays "
              << formatCOP(student.fareOn(arvi)) << " (no flat fare there)\n\n";

    const std::vector<ChargedLeg> charged = chargeItinerary(itinerary, frequent);

    std::cout << ">> 3. CHARGED ITINERARY (frequent profile)\n";
    std::for_each(charged.begin(), charged.end(), [](const ChargedLeg& c) {
        std::cout << "  min " << c.minute << " | " << c.serviceType << " " << c.modeName
                  << " | " << formatCOP(c.charge)
                  << (c.integrated ? "  [free transfer]" : "") << '\n';
    });
    std::cout << '\n';

    std::cout << ">> 4. REPORTS BUILT WITH STL ALGORITHMS\n";
    std::cout << "  accumulate: total paid -> " << formatCOP(totalPaid(charged)) << '\n';
    std::cout << "  count_if: free transfers -> " << freeTransferCount(charged) << '\n';

    const auto byType = paidByServiceType(charged);
    std::for_each(byType.begin(), byType.end(),
        [](const std::pair<const std::string, long long>& entry) {
            std::cout << "  map: paid on " << entry.first << " -> "
                      << formatCOP(entry.second) << '\n';
        });

    const auto firstPaid = std::find_if(charged.begin(), charged.end(),
        [](const ChargedLeg& c) { return c.charge > 0; });
    std::cout << "  find_if: first leg that actually paid -> "
              << (firstPaid == charged.end() ? std::string("none") : firstPaid->modeName) << '\n';

    const std::function<bool(const TransportMode&)> bigModes =
        [](const TransportMode& m) { return m.stations() > 8; };
    std::cout << "  injected policy: modes with more than 8 stations -> "
              << joined(namesMatching(network, bigModes)) << '\n';

    std::cout << "  total for the student on the same itinerary -> "
              << formatCOP(totalPaid(chargeItinerary(itinerary, student))) << '\n';
    std::cout << "  total for the senior on the same itinerary -> "
              << formatCOP(totalPaid(chargeItinerary(itinerary, senior))) << '\n';

    std::cout << "\n============================================================\n";
    std::cout << "  End of demonstration.\n";
    std::cout << "============================================================\n";
    return 0;
}
